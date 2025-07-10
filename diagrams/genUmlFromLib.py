from asyncio import subprocess
import asyncio
import shutil
import sys
import os
from enum import Enum
from types import CoroutineType
from typing import Callable

import hpp2plantuml

class Direction(Enum):
    LEFT_TO_RIGHT = "left to right direction"
    TOP_TO_BOTTOM = "top to bottom direction"

CORE_DIRECTORY = os.path.join(os.path.curdir, "../lib/URM/Core/")
ENGINE_DIRECTORY = os.path.join(os.path.curdir, "../lib/URM/Engine/")
MODULE_BASE_DIRECTORY = os.path.join(os.path.curdir, "../lib/URM/")

OUTPUT_DIRECTORY = os.path.join(os.path.curdir, "./generator_output/")

IGNORE_FILES = [
    "VertexConcept.h",
    "pch.h",
    "framework.h",
]

class Module:
    def __init__(self, name: str, direction: Direction, files_list: list[str], custom_uml_processor: Callable[[list[str]], list[str]] = lambda x: x):
        self.name = name
        self.files_list = files_list
        self.direction = direction
        self.custom_uml_processor = custom_uml_processor

def scene_objects_uml_processor(lines_list: list[str]) -> list[str]:
    # Custom processing for SceneObjects module
    processed_lines = []
    is_in_scene_object = False
    for line in lines_list:
        if "class SceneObject {" in line:
            is_in_scene_object = True
            processed_lines.append(line)
            processed_lines.append("\t\t...\n")  # Add a separator line
        elif is_in_scene_object and line.strip() == "}":
            is_in_scene_object = False
            processed_lines.append(line)
        elif not is_in_scene_object:
            processed_lines.append(line)

    return processed_lines

def scene_objects_model_uml_processor(lines_list: list[str]) -> list[str]:
    lines_list = scene_objects_uml_processor(lines_list)
    found_mesh_constructor = False
    found_model_constructor = False
    for (i, line) in enumerate(lines_list):
        if line.strip() == "+MeshObject(const std::shared_ptr<Core::Mesh<Core::ModelLoaderVertexType>>& mesh, const std::shared_ptr<Core::VertexShader>& vertexShader, const std::shared_ptr<Core::D3DInputLayout<Core::ModelLoaderVertexType>>& inputLayout, const std::shared_ptr<URM::Core::Material> material)":
            lines_list[i] = "+MeshObject(mesh, vertexShader, inputLayout, material)\n"
            found_mesh_constructor = True

        if line.strip() == "+ModelObject(const std::string& path, const std::shared_ptr<Core::Material>& material, const std::shared_ptr<Core::VertexShader>& vertexShader, const std::shared_ptr<Core::ModelLoaderLayout>& layout)":
            lines_list[i] = "+ModelObject(path, material, vertexShader, layout)\n"
            found_model_constructor = True

    if not found_mesh_constructor:
        raise ValueError("MeshObject constructor not found in the UML lines. Please check the input files for changes.")

    if not found_model_constructor:
        raise ValueError("ModelObject constructor not found in the UML lines. Please check the input files for changes.")

    return lines_list

def utils_uml_processor(lines_list: list[str]) -> list[str]:
    # Custom processing for Utils module
    processed_lines = []
    in_non_copyable = False
    for line in lines_list:
        if "class NonCopyable {" in line:
            in_non_copyable = True
        elif in_non_copyable and line.strip() == "}":
            in_non_copyable = False
        elif not in_non_copyable:
            processed_lines.append(line)

    return processed_lines

def d3dutils2_uml_processor(lines_list: list[str]) -> list[str]:
    # Custom processing for D3DUtils2 module
    processed_lines = []

    D3DEPTH_STENCIL_STATE_CLASS = "D3DDepthStencilStateData::D3DDepthStencilState"
    for line in lines_list:
        if D3DEPTH_STENCIL_STATE_CLASS in line:
            line = line.replace(D3DEPTH_STENCIL_STATE_CLASS, "D3DDepthStencilState")

        processed_lines.append(line)

    return processed_lines

ENABLE_PLANTUML_IMAGE_GENERATOR = True
REMOVE_OLD_FILES = True
MODULES = [
    Module("Core", Direction.LEFT_TO_RIGHT, ["Core/D3DCore.h", "Core/Window.h"]),
    Module("D3DUtils", Direction.LEFT_TO_RIGHT, ["Core/D3DViewport.h", "Core/D3DRasterizerState.h", "Core/D3DBlendState.h", "Core/D3DDepthStencilState.h"], custom_uml_processor=d3dutils2_uml_processor),
    Module("Buffer", Direction.LEFT_TO_RIGHT, ["Core/ID3DBuffer.h", "Core/D3DConstantBuffer.h", "Core/D3DIndexBuffer.h", "Core/D3DVertexBuffer.h"]),
    Module("Mesh", Direction.TOP_TO_BOTTOM, ["Core/IMesh.h", "Core/Mesh.h", "Core/MaterialProperty.h"]),
    Module("ModelLoader", Direction.LEFT_TO_RIGHT, ["Core/ModelLoader.h"]),
    Module("Texture", Direction.LEFT_TO_RIGHT, ["Core/D3DTexture2D.h", "Core/D3DSampler.h"]),
    Module("Shader", Direction.LEFT_TO_RIGHT, ["Core/ShaderPipeline.h",  "Core/D3DInputLayout.h"]),
    Module("Material", Direction.LEFT_TO_RIGHT, ["Core/Material.h"]),
    Module("VertexTypes", Direction.LEFT_TO_RIGHT, ["Core/StandardVertexTypes.h"]),
    Module("Utils", Direction.LEFT_TO_RIGHT, ["Core/Utils.h"], custom_uml_processor=utils_uml_processor),
    Module("Logging", Direction.LEFT_TO_RIGHT, ["Core/Log.h"]),
    Module("Stopwatch", Direction.LEFT_TO_RIGHT, ["Core/Stopwatch.h"]),
    Module("Engine", Direction.LEFT_TO_RIGHT, ["Engine/Engine.h", "Engine/Timer.h"]),
    Module("Scene", Direction.LEFT_TO_RIGHT, ["Engine/Scene.h"]),
    Module("SceneObject", Direction.LEFT_TO_RIGHT, ["Engine/SceneObject.h", "Engine/Transform.h"]),
    Module("sceneobjects_model", Direction.LEFT_TO_RIGHT, ["Engine/SceneObject.h", "Engine/MeshObject.h", "Engine/ModelObject.h"], custom_uml_processor=scene_objects_model_uml_processor),
    Module("sceneobjects_light", Direction.LEFT_TO_RIGHT, ["Engine/SceneObject.h", "Engine/LightObject.h"], custom_uml_processor=scene_objects_uml_processor),
    Module("sceneobjects_camera", Direction.LEFT_TO_RIGHT, ["Engine/SceneObject.h", "Engine/CameraObject.h", "Engine/FlyCameraObject.h"], custom_uml_processor=scene_objects_uml_processor),
    Module("Assets", Direction.LEFT_TO_RIGHT, ["Engine/AssetManager.h"]),
]

def normalizePath(path: str) -> str:
    return os.path.normpath(path).replace("\\", "/")

async def generateImageForUMLFile(uml_file: str) -> subprocess.Process:
    if ENABLE_PLANTUML_IMAGE_GENERATOR:
        print(f"Generating image for {uml_file}...")
        # os.system(f"plantuml {uml_file}")
        p = await asyncio.create_subprocess_shell(f"echo Generating {uml_file} file... && plantuml {uml_file}", shell=True)
        return p

    return await asyncio.Future()  # Return a dummy future if image generation is disabled

def rewrutePumlFile(module: Module, uml_file: str, output_file: str):
    with open(uml_file, 'r') as file:
        content = file.readlines()

    for i, line in enumerate(content):
        # Remove self references
        if "--" in line:
            splitted = line.strip().split(" ")
            if len(splitted) >= 3 and splitted[0] == splitted[-1]:
                content[i] = ''


    # Skip @startuml
    content.insert(1, f"{module.direction.value}\n")

    content = module.custom_uml_processor(content)

    with open(output_file, 'w') as file:
        file.writelines(content)

async def processModules(all_files: list[str]):
    if not os.path.exists(OUTPUT_DIRECTORY):
        os.makedirs(OUTPUT_DIRECTORY)

    puml_files = []
    for m in MODULES:
        file_paths = [normalizePath(os.path.join(MODULE_BASE_DIRECTORY, f)) for f in m.files_list if f.endswith('.h') and f not in IGNORE_FILES]
        output_file = os.path.join(OUTPUT_DIRECTORY, f"{m.name.lower()}.puml")
        temp_file = output_file + ".tmp"

        for f in file_paths:
            if not os.path.isfile(f):
                print(f"File {f} does not exist!")
                sys.exit(1)

        print(f"Generating UML file {output_file}...")
        hpp2plantuml.CreatePlantUMLFile(file_paths, temp_file)
        rewrutePumlFile(m, temp_file, output_file)
        if os.path.exists(temp_file):
            os.remove(temp_file)
        puml_files.append(output_file)

        for f in file_paths:
            if f in all_files:
                all_files.remove(f)

    async_list: list[subprocess.Process] = []
    for p in puml_files:
        async_list.append(await generateImageForUMLFile(p))

    for future in async_list:
        try:
            await future.communicate()
        except Exception as e:
            print(f"Error generating image: {e}")

    print("Not used files:")
    for f in all_files:
        realtive_path = os.path.relpath(f, MODULE_BASE_DIRECTORY)
        print("-", realtive_path)

def getAllFiles() -> list[str]:
    if REMOVE_OLD_FILES:
        shutil.rmtree(OUTPUT_DIRECTORY, ignore_errors=True)

    if not os.path.exists(OUTPUT_DIRECTORY):
        os.makedirs(OUTPUT_DIRECTORY)

    core_files = [os.path.join(CORE_DIRECTORY, f) for f in os.listdir(CORE_DIRECTORY) if f.endswith('.h') and f not in IGNORE_FILES]
    # core_uml_file = os.path.join(OUTPUT_DIRECTORY, "ALL_core.puml")
    # hpp2plantuml.CreatePlantUMLFile(core_files, core_uml_file)
    # generateImageForUMLFile(core_uml_file)

    engine_files = [os.path.join(ENGINE_DIRECTORY, f) for f in os.listdir(ENGINE_DIRECTORY) if f.endswith('.h') and f not in IGNORE_FILES]
    # engine_uml_file = os.path.join(OUTPUT_DIRECTORY, "ALL_engine.puml")
    # hpp2plantuml.CreatePlantUMLFile(engine_files, engine_uml_file)
    # generateImageForUMLFile(engine_uml_file)

    all_files = core_files + engine_files
    all_files = [normalizePath(f) for f in all_files if os.path.isfile(f)]
    # all_uml_file = os.path.join(OUTPUT_DIRECTORY, "ALL.puml")
    # hpp2plantuml.CreatePlantUMLFile(all_files, all_uml_file)
    # generateImageForUMLFile(all_uml_file)
    return all_files

async def main():
    all_files = getAllFiles()
    await processModules(all_files)

if __name__ == "__main__":
    asyncio.run(main())
    print("Done!")