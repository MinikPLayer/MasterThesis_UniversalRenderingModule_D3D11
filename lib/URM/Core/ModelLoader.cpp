#include "pch.h"
#include "ModelLoader.h"
#include "D3DTexture2D.h"
#include "Mesh.h"
#include "MaterialProperty.h"

#include <assimp/mesh.h>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

//void ModelLoader::Draw(ID3D11DeviceContext* devcon) {
//	for (size_t i = 0; i < meshes_.size(); ++i) {
//		meshes_[i].Draw(devcon);
//	}
//}
//

D3DTexture2D LoadEmbeddedTexture(D3DCore& core, std::string path, std::string type, const aiTexture* embeddedTexture) {
	auto width = embeddedTexture->mWidth;
	auto height = embeddedTexture->mHeight;
	auto pixelData = embeddedTexture->pcData;

	return D3DTexture2D(core, path, type, Size2i(width, height), reinterpret_cast<Texel2D*>(pixelData));
}

std::vector<D3DTexture2D> LoadMaterialTextures(D3DCore& core, std::vector<D3DTexture2D>& loadedTexturesPool, std::string fileDirectory, aiMaterial* mat, aiTextureType type, std::string typeName, const aiScene* scene) {
	std::vector<D3DTexture2D> textures;
	for (UINT i = 0; i < mat->GetTextureCount(type); i++) {
		aiString str;
		mat->GetTexture(type, i, &str);
		// Check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
		bool skip = false;
		for (UINT j = 0; j < loadedTexturesPool.size(); j++) {
			if (std::strcmp(loadedTexturesPool[j].GetPath().c_str(), str.C_Str()) == 0) {
				textures.push_back(loadedTexturesPool[j]);
				skip = true; // A texture with the same filepath has already been loaded, continue to next one. (optimization)
				break;
			}
		}
		if (!skip) {   // If texture hasn't been loaded already, load it
			const aiTexture* embeddedTexture = scene->GetEmbeddedTexture(str.C_Str());

			D3DTexture2D texture = embeddedTexture == nullptr ? 
				D3DTexture2D(core, fileDirectory + '/' + std::string(str.C_Str()), typeName) :
				LoadEmbeddedTexture(core, std::string(str.C_Str()), typeName, embeddedTexture);

			textures.push_back(texture);
			loadedTexturesPool.push_back(texture);  // Store it as texture loaded for entire model, to ensure we won't unnecesery load duplicate textures.
		}
	}
	return textures;
}


float unpackFloat(const uint8_t* b) {
	uint32_t temp = 0;
	temp = (((uint32_t)b[3] << 24) |
		((uint32_t)b[2] << 16) |
		((uint32_t)b[1] << 8) |
		(uint32_t)b[0]);
	return *((float*)&temp);
}

double unpackDouble(const uint8_t* b) {
	uint64_t temp = 0;
	temp = ((uint64_t)b[7] << 56) |
		((uint64_t)b[6] << 48) |
		((uint64_t)b[5] << 40) |
		((uint64_t)b[4] << 32) |
		((uint64_t)b[3] << 24) |
		((uint64_t)b[2] << 16) |
		((uint64_t)b[1] << 8) |
		b[0];
	return *((double*)&temp);
}

int unpackInteger(const uint8_t* b) {
	int temp = ((b[3] << 24) |
		(b[2] << 16) |
		(b[1] << 8) |
		b[0]);
	return temp;
}

template<typename T, int TSizeInBytes>
std::vector<T> unpackPropertyData(const unsigned char* buf, unsigned int bufSizeInBytes, std::function<T(const unsigned char*)> unpackFunc) {

	std::vector<T> temp;
	for (size_t offset = 0; offset < bufSizeInBytes; offset += TSizeInBytes) {
		auto value = unpackFunc(buf + offset);
		temp.push_back(value);
	}
	return temp;
}

MaterialProperty GetPropertyFromAssimpProperty(aiMaterialProperty* prop) {
	auto nameString = std::string(prop->mKey.C_Str());

	switch (prop->mType) {
	case aiPTI_String:
		// Skip first 4 bytes, which are the length of the string + 1 byte for the null terminator
		// TODO: Verify prop->mDataLength vs first 4 bytes
		return MaterialProperty::CreateString(nameString, prop->mData + 4, prop->mDataLength - 4 - 1);

	case aiPTI_Float:
		return MaterialProperty::CreateFloat(nameString, unpackPropertyData<float, 4>((const unsigned char*)prop->mData, prop->mDataLength, unpackFloat));
	
	case aiPTI_Double:
		return MaterialProperty::CreateDouble(nameString, unpackPropertyData<double, 8>((const unsigned char*)prop->mData, prop->mDataLength, unpackDouble));

	case aiPTI_Integer:
		return MaterialProperty::CreateInteger(nameString, unpackPropertyData<int, 4>((const unsigned char*)prop->mData, prop->mDataLength, unpackInteger));

	case aiPTI_Buffer:
	{
		unsigned char* newData = new unsigned char[prop->mDataLength];
		std::copy(prop->mData, prop->mData + prop->mDataLength, newData);
		return MaterialProperty::CreateBuffer(nameString, newData, prop->mDataLength);
	}

	default:
		throw std::runtime_error("Unknown property type");
	}
}

Mesh<LoaderVertexType> processMesh(D3DCore& core, std::vector<D3DTexture2D>& loadedTexturesPool, std::string fileDirectory, aiMesh* mesh, const aiScene* scene) {
	// Data to fill
	std::vector<LoaderVertexType> vertices;
	std::vector<UINT> indices;

	// Walk through each of the mesh's vertices
	for (UINT i = 0; i < mesh->mNumVertices; i++) {
		LoaderVertexType vertex;

		vertex.position.x = mesh->mVertices[i].x;
		vertex.position.y = mesh->mVertices[i].y;
		vertex.position.z = mesh->mVertices[i].z;

		if (mesh->mTextureCoords[0]) {
			vertex.texcoord.x = (float)mesh->mTextureCoords[0][i].x;
			vertex.texcoord.y = (float)mesh->mTextureCoords[0][i].y;
		}

		vertices.push_back(vertex);
	}

	for (UINT i = 0; i < mesh->mNumFaces; i++) {
		aiFace face = mesh->mFaces[i];

		for (UINT j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}

	// TODO: Add support for normals and tangents

	std::vector<MaterialProperty> materialProperties;
	std::vector<D3DTexture2D> textures;
	if (scene->mNumMaterials > 0) {
		auto mat = scene->mMaterials[mesh->mMaterialIndex];

		for (size_t i = 0; i < mat->mNumProperties; i++) {
			auto prop = mat->mProperties[i];
			materialProperties.push_back(GetPropertyFromAssimpProperty(prop));
		}

		std::vector<D3DTexture2D> diffuseMaps = LoadMaterialTextures(core, loadedTexturesPool, fileDirectory, mat, aiTextureType_DIFFUSE, "texture_diffuse", scene);
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
	}

	//// Test
	//for (auto prop : newMesh.materialProperties) {
	//	OutputDebugString(StringUtils::StringToWString(prop.name + ": " + prop.GetValueAsString() + "\n").c_str());
	//}

	auto newMesh = Mesh<LoaderVertexType>(core, vertices, indices, textures);
	newMesh.materialProperties = materialProperties;

	return newMesh;
}

ModelLoaderNode processNode(D3DCore& core, std::vector<D3DTexture2D>& loadedTexturesPool, std::string fileDirectory, aiNode* node, const aiScene* scene) {
	ModelLoaderNode newNode;
	if (node->mTransformation.IsIdentity()) {
		newNode.transform = DirectX::XMMatrixIdentity();
	}
	else {
		aiMatrix4x4 transform = node->mTransformation;
		newNode.transform = DirectX::XMMatrixTranspose(DirectX::XMLoadFloat4x4(reinterpret_cast<DirectX::XMFLOAT4X4*>(&transform)));
	}

	for (UINT i = 0; i < node->mNumMeshes; i++) {
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		newNode.meshes.push_back(processMesh(core, loadedTexturesPool, fileDirectory, mesh, scene));
	}

	for (UINT i = 0; i < node->mNumChildren; i++) {
		auto newChild = processNode(core, loadedTexturesPool, fileDirectory, node->mChildren[i], scene);
		newNode.children.push_back(newChild);
	}

	return newNode;
}

std::optional<ModelLoaderNode> Load(D3DCore& core, std::vector<D3DTexture2D>& loadedTexturesPool, std::string filePath) {
	Assimp::Importer importer;

	const aiScene* pScene = importer.ReadFile(filePath,
		aiProcess_Triangulate |
		aiProcess_ConvertToLeftHanded);

	if (pScene == nullptr)
		return std::nullopt;

	auto dir = StringUtils::GetDirectoryFromPath(filePath);
	return processNode(core, loadedTexturesPool, dir, pScene->mRootNode, pScene);
}

ModelLoaderNode ModelLoader::LoadFromFile(D3DCore& core, std::vector<D3DTexture2D>& loadedTexturesPool, std::string path) {
	auto mesh = Load(core, loadedTexturesPool, path);
	if (mesh.has_value())
		return mesh.value();
	else
		throw std::runtime_error("Failed to load model from file: " + path);
}