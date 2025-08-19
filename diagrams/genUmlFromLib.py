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

def unpack_sharp_brackets(line: str, prefix: str, replace_with_suffix: str = "") -> str:
    index = line.find(prefix + "<")
    while index != -1:
        start_index = index + len(prefix) + 1  # +1 for '<'
        stack_size = 1
        for i in range(start_index, len(line)):
            if line[i] == '<':
                stack_size += 1
            elif line[i] == '>':
                stack_size -= 1

            if stack_size == 0:
                end_index = i + 1
                break
        else:
            print(f"Error: Unmatched '<' in line: {line}")
            return line  # Return the original line if no match is found
        
        contents = line[start_index:end_index - 1].strip()
        line = line[:index] + contents + replace_with_suffix + line[end_index:]
        index = line.find(prefix + "<", index + len(contents) + len(replace_with_suffix))

    return line

def find_next_sharp_bracket(line: str, start_index: int) -> int:
    stack_size = 0
    if start_index < len(line) and line[start_index] != '<':
        stack_size = 1
    for i in range(start_index, len(line)):
        if line[i] == '<':
            stack_size += 1
        elif line[i] == '>':
            stack_size -= 1

        if stack_size == 0:
            return i

    return -1  # No matching '>' found

def vector_to_array(line: str) -> str:
    index = line.find("vector<")
    while index != -1:
        end_index = find_next_sharp_bracket(line, index + 6)  # Find the matching '>'
        if end_index == -1:
            print(f"Error: Unmatched '>' in vector parameter: {line[index:]}")
            break

        content = line[index + 7:end_index].strip()  # Extract the content inside vector<>
        line = line[:index] + content + "[]" + line[end_index + 1:]  # Append the rest of the string after '>'

        index = line.find("vector<", end_index + 1)

    return line 

def split_params(line: str) -> list[str]:
    params = [""]
    sharp_bracket_stack = 0
    for l in line:
        if l == '<':
            sharp_bracket_stack += 1
        elif l == '>':
            sharp_bracket_stack -= 1
        
        if l == ',' and sharp_bracket_stack == 0:
            params.append("")
        else:
            params[-1] += l

    return [param.strip() for param in params if param.strip()]

def shorten_params(line: str) -> str:
    start_index = line.find("(")
    end_index = line.find(")", start_index)
    if start_index == -1 or end_index == -1:
        return line
    
    params = split_params(line[start_index + 1:end_index].strip())
    shortened_params = []
    for param in params:
        param = param.strip()
        if param == "":
            continue

        shortened_param = param.split()[-1]  # Take the last part of the parameter (type)
        shortened_params.append(shortened_param)

    shortened_params_str = ", ".join(shortened_params)
    return line[:start_index + 1] + shortened_params_str + line[end_index:]

def shorten_line_parameters(line: str) -> str:
    # Stage 1 - unpack sharp brackets
    line = unpack_sharp_brackets(line, "shared_ptr", "*")
    line = unpack_sharp_brackets(line, "unique_ptr", "*")
    line = unpack_sharp_brackets(line, "weak_ptr", "*")
    line = unpack_sharp_brackets(line, "ComPtr", "*")
    line = unpack_sharp_brackets(line, "optional", "?")
    line = unpack_sharp_brackets(line, "vector", "[]")

    line = line.replace("const ", "").replace("constexpr ", "").replace("*&", "*").replace("URM::", "").replace("Core::D3DCore", "D3DCore")

    return line

def texture_uml_processor(lines_list: list[str]) -> list[str]:
    processed_line = []
    found_create_from_file = False
    found_create_from_memory = False
    for line in lines_list:
        if "+{static} CreateFromFile(D3DCore& core, string& path, string& type, D3DTexture2DCreationParams& params) : D3DTexture2D" in line:
            line = "+CreateFromFile(core, path, type, params) : D3DTexture2D\n"
            found_create_from_file = True

        if "+{static} CreateFromMemory(D3DCore& core, string& type, Size2i size, Texel2D* pixelData, D3DTexture2DCreationParams& params) : D3DTexture2D" in line:
            line = "+CreateFromMemory(core, type, size, pixelData, params) : D3DTexture2D\n"
            found_create_from_memory = True

        processed_line.append(line)

    if not found_create_from_memory:
        raise ValueError("CreateFromMemory method not found in the Texture UML lines. Please check the input files for changes.")
    
    if not found_create_from_file:
        raise ValueError("CreateFromFile method not found in the Texture UML lines. Please check the input files for changes.")
    
    return processed_line

def core_uml_processor(lines_list: list[str]) -> list[str]:
    processed_lines = []
    for line in lines_list:
        line = line.replace("-WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) : LRESULT", "-WndProc(...) : LRESULT") \
                .replace("-WndProdDispatcher(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) : LRESULT", "-WndProdDispatcher(...) : LRESULT")
        processed_lines.append(line)

    return processed_lines

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
    found_deduce_material = False
    for (i, line) in enumerate(lines_list):
        if line.strip().startswith("+MeshObject("):
            lines_list[i] = shorten_line_parameters(line)
            lines_list[i] = shorten_params(lines_list[i])
            found_mesh_constructor = True

        if line.strip().startswith("+ModelObject("):
            lines_list[i] = shorten_line_parameters(line)
            lines_list[i] = shorten_params(lines_list[i])
            found_model_constructor = True

        if "-TryDeduceMaterial(D3DCore& core, Core::MaterialProperty[] properties) : Core::Material*" in line:
            lines_list[i] = shorten_line_parameters(line)
            found_deduce_material = True

    if not found_deduce_material:
        raise ValueError("TryDeduceMaterial method not found in the UML lines. Please check the input files for changes.")

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
        line = line.replace("<long long , ratio<1 , 1000000000>>", "").replace("<long long, ratio<1, 1000000000>>", "")

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

def buffers_uml_processor(lines_list: list[str]) -> list[str]:
    # Custom processing for Buffers module
    processed_lines = []

    is_in_id3dbuffer = False
    for line in lines_list:
        line = shorten_params(line)

        if "abstract class ID3DBuffer {" in line:
            is_in_id3dbuffer = True
            processed_lines.append(line)
            processed_lines.append("\t\t...\n")  # Add a separator line
            processed_lines.append("}")
        elif is_in_id3dbuffer and line.strip() == "}":
            is_in_id3dbuffer = False
        elif not is_in_id3dbuffer:
            processed_lines.append(line)

    return processed_lines

def constant_buffers_uml_processor(lines_list: list[str]) -> list[str]:
    # Custom processing for ConstantBuffers module
    processed_lines = []
    found_constructor = False
    for line in lines_list:
        if "+PixelLight(Vector3 position, Color color, float ambient, float diffuse, float specular, float attenuationExponent, float pbrIntensity)" in line:
            line = "+PixelLight(position, color, [params])\n"
            found_constructor = True
        processed_lines.append(line)

    if not found_constructor:
        raise ValueError("PixelLight constructor not found in the UML lines. Please check the input files for changes.")

    return processed_lines

def engine_uml_processor(lines_list: list[str]) -> list[str]:
    # Custom processing for Engine module
    processed_lines = []
    for line in lines_list:
        line = line.replace(
            "+Draw(RenderingParams& params, std::weak_ptr<CameraObject> mainCamera, std::vector<std::weak_ptr<MeshObject>>& meshes, std::vector<std::weak_ptr<LightObject>>& lights) : void",
            "+Draw(RenderingParams& params, CameraObject* mainCamera, vector<MeshObject*>& meshes, vector<LightObject*>& lights)"
        ).replace(
            "+function<void(Engine& engine)",
            "+onUpdate : std::function<void(Engine&)>"
        )

        processed_lines.append(line)

    return processed_lines

def modelloader_uml_processor(lines_list: list[str]) -> list[str]:
    # Custom processing for ModelLoader module
    processed_lines = []
    for line in lines_list:
        if "+{static} LoadFromFile(" in line:
            line = shorten_params(line)

        processed_lines.append(line)

    return processed_lines

ENABLE_PLANTUML_IMAGE_GENERATOR = True
REMOVE_OLD_FILES = True
MODULES = [
    Module("Core", Direction.LEFT_TO_RIGHT, ["Core/D3DCore.h", "Core/Window.h"], custom_uml_processor=core_uml_processor),
    Module("D3DUtils_1", Direction.LEFT_TO_RIGHT, ["Core/D3DViewport.h", "Core/D3DRasterizerState.h"], custom_uml_processor=d3dutils2_uml_processor),
    Module("D3DUtils_2", Direction.LEFT_TO_RIGHT, ["Core/D3DBlendState.h", "Core/D3DDepthStencilState.h"], custom_uml_processor=d3dutils2_uml_processor),
    Module("Buffer", Direction.LEFT_TO_RIGHT, ["Core/ID3DBuffer.h"]),
    Module("Buffers", Direction.LEFT_TO_RIGHT, ["Core/ID3DBuffer.h", "Core/D3DConstantBuffer.h", "Core/D3DIndexBuffer.h", "Core/D3DVertexBuffer.h"], custom_uml_processor=buffers_uml_processor),
    Module("Mesh", Direction.TOP_TO_BOTTOM, ["Core/IMesh.h", "Core/Mesh.h", "Core/MaterialProperty.h"]),
    Module("ModelLoader", Direction.LEFT_TO_RIGHT, ["Core/ModelLoader.h"], custom_uml_processor=modelloader_uml_processor),
    Module("Texture", Direction.LEFT_TO_RIGHT, ["Core/D3DTexture2D.h", "Core/D3DSampler.h"], custom_uml_processor=texture_uml_processor),
    Module("Shader", Direction.LEFT_TO_RIGHT, ["Core/ShaderPipeline.h", "Core/D3DInputLayout.h"]),
    Module("Materials", Direction.TOP_TO_BOTTOM, ["Core/Material.h", "Core/StandardMaterials.h"]),
    Module("VertexTypes", Direction.LEFT_TO_RIGHT, ["Core/StandardVertexTypes.h"]),
    Module("Utils", Direction.LEFT_TO_RIGHT, ["Core/Utils.h"], custom_uml_processor=utils_uml_processor),
    Module("Logging", Direction.LEFT_TO_RIGHT, ["Core/Log.h"]),
    Module("Stopwatch", Direction.LEFT_TO_RIGHT, ["Core/Stopwatch.h"]),
    Module("Engine", Direction.TOP_TO_BOTTOM, ["Engine/Engine.h", "Engine/Timer.h"], custom_uml_processor=engine_uml_processor),
    Module("ConstantBuffers", Direction.LEFT_TO_RIGHT, ["Engine/ConstantBufferTypes.h"], custom_uml_processor=constant_buffers_uml_processor),
    Module("Scene", Direction.LEFT_TO_RIGHT, ["Engine/Scene.h"]),
    Module("SceneObject", Direction.LEFT_TO_RIGHT, ["Engine/SceneObject.h", "Engine/Transform.h"]),
    Module("sceneobjects_model", Direction.TOP_TO_BOTTOM, ["Engine/SceneObject.h", "Engine/MeshObject.h", "Engine/ModelObject.h"], custom_uml_processor=scene_objects_model_uml_processor),
    Module("sceneobjects_light", Direction.LEFT_TO_RIGHT, ["Engine/SceneObject.h", "Engine/LightObject.h"], custom_uml_processor=scene_objects_uml_processor),
    Module("sceneobjects_camera", Direction.TOP_TO_BOTTOM, ["Engine/SceneObject.h", "Engine/CameraObject.h", "Engine/FlyCameraObject.h"], custom_uml_processor=scene_objects_uml_processor),
    Module("Assets", Direction.LEFT_TO_RIGHT, ["Engine/AssetManager.h"]),
    Module("Tests", Direction.TOP_TO_BOTTOM, ["../../URMBenchmarks/ITest.h", "../../URMBenchmarks/AverageAccumulator.h"])
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
                line = ""

        line = line.replace("std::chrono::", "").replace("std::", "")
        line = shorten_line_parameters(line)
        content[i] = line


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