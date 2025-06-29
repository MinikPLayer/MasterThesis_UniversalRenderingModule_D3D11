import shutil
import sys
import os
from enum import Enum

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
    "pch.h"
]

class Module:
    def __init__(self, name: str, direction: Direction, files_list: list[str]):
        self.name = name
        self.files_list = files_list
        self.direction = direction

    @staticmethod
    def create_left_to_right(name: str, files_list: list[str]):
        return Module(name, Direction.LEFT_TO_RIGHT, files_list)

    @staticmethod
    def create_top_to_bottom(name: str, files_list: list[str]):
        return Module(name, Direction.TOP_TO_BOTTOM, files_list)

ENABLE_PLANTUML_IMAGE_GENERATOR = True
REMOVE_OLD_FILES = True
MODULES = [
    Module.create_left_to_right("Core", ["Core/D3DCore.h", "Core/Window.h"]),
    Module.create_top_to_bottom("D3DUtils", ["Core/D3DViewport.h", "Core/D3DRasterizerState.h"]),
    Module.create_top_to_bottom("Buffer", ["Core/ID3DBuffer.h", "Core/D3DConstantBuffer.h", "Core/D3DIndexBuffer.h", "Core/D3DVertexBuffer.h"]),
    Module.create_top_to_bottom("Mesh", ["Core/IMesh.h", "Core/Mesh.h", "Core/MaterialProperty.h"]),
    Module.create_left_to_right("ModelLoader", ["Core/ModelLoader.h"]),
    Module.create_top_to_bottom("Texture", ["Core/D3DTexture2D.h", "Core/D3DSampler.h"]),
    Module.create_left_to_right("Shader", ["Core/ShaderProgram.h",  "Core/D3DInputLayout.h"]),
    Module.create_left_to_right("VertexTypes", ["Core/StandardVertexTypes.h"]),
    Module.create_left_to_right("Utils", ["Core/Utils.h"]),
    Module.create_left_to_right("Logging", ["Core/Log.h"]),
    Module.create_left_to_right("Stopwatch", ["Core/Stopwatch.h"]),
]

def normalizePath(path: str) -> str:
    return os.path.normpath(path).replace("\\", "/")

def generateImageForUMLFile(uml_file: str):
    if ENABLE_PLANTUML_IMAGE_GENERATOR:
        print(f"Generating image for {uml_file}...")
        os.system(f"plantuml {uml_file}")

def rewrutePumlFile(module: Module, uml_file: str, output_file: str):
    with open(uml_file, 'r') as file:
        content = file.readlines()

    # Skip @startuml
    content.insert(1, f"{module.direction.value}\n")

    with open(output_file, 'w') as file:
        file.writelines(content)

def processModules(all_files: list[str]):
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

    print("Not used files:")
    for f in all_files:
        realtive_path = os.path.relpath(f, MODULE_BASE_DIRECTORY)
        print("-", realtive_path)

    for p in puml_files:
        generateImageForUMLFile(p)

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

def main():
    all_files = getAllFiles()
    processModules(all_files)

if __name__ == "__main__":
    main()