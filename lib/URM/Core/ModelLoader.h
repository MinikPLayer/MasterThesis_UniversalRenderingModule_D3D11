#pragma once

#include "Mesh.h"
#include "VertexPosition.h"
#include "D3DCore.h"

using LoaderVertexType = VertexPositionTexture;

class ModelLoaderNode {
public:
	std::vector<Mesh<LoaderVertexType>> meshes;

	std::vector<ModelLoaderNode> children;
};

class ModelLoader {
	ModelLoader() {}

public:

	static ModelLoaderNode LoadFromFile(D3DCore& core, std::string path);
};