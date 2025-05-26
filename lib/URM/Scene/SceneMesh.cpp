#include "pch.h"
#include "SceneMesh.h"

SceneMesh::SceneMesh(Scene& scene, Mesh<ModelLoaderVertexType> mesh, std::string name) :SceneObject(scene, name), mesh(mesh) {}
