#include "pch.h"
#include "ModelLoader.h"
#include "D3DTexture.h"
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
//std::vector<Texture> ModelLoader::loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName, const aiScene* scene) {
//	std::vector<Texture> textures;
//	for (UINT i = 0; i < mat->GetTextureCount(type); i++) {
//		aiString str;
//		mat->GetTexture(type, i, &str);
//		// Check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
//		bool skip = false;
//		for (UINT j = 0; j < textures_loaded_.size(); j++) {
//			if (std::strcmp(textures_loaded_[j].path.c_str(), str.C_Str()) == 0) {
//				textures.push_back(textures_loaded_[j]);
//				skip = true; // A texture with the same filepath has already been loaded, continue to next one. (optimization)
//				break;
//			}
//		}
//		if (!skip) {   // If texture hasn't been loaded already, load it
//			HRESULT hr;
//			Texture texture;
//
//			const aiTexture* embeddedTexture = scene->GetEmbeddedTexture(str.C_Str());
//			if (embeddedTexture != nullptr) {
//				texture.texture = loadEmbeddedTexture(embeddedTexture);
//			}
//			else {
//				std::string filename = std::string(str.C_Str());
//				filename = directory_ + '/' + filename;
//				std::wstring filenamews = std::wstring(filename.begin(), filename.end());
//				DX::ThrowIfFailed(
//					CreateWICTextureFromFile(dev_, devcon_, filenamews.c_str(), nullptr, &texture.texture),
//					"WIC Texture creation failed."
//				);
//			}
//			texture.type = typeName;
//			texture.path = str.C_Str();
//			textures.push_back(texture);
//			this->textures_loaded_.push_back(texture);  // Store it as texture loaded for entire model, to ensure we won't unnecesery load duplicate textures.
//		}
//	}
//	return textures;
//}

//ID3D11ShaderResourceView* ModelLoader::loadEmbeddedTexture(const aiTexture* embeddedTexture) {
//	HRESULT hr;
//	ID3D11ShaderResourceView* texture = nullptr;
//
//	if (embeddedTexture->mHeight != 0) {
//		// Load an uncompressed ARGB8888 embedded texture
//		D3D11_TEXTURE2D_DESC desc;
//		desc.Width = embeddedTexture->mWidth;
//		desc.Height = embeddedTexture->mHeight;
//		desc.MipLevels = 1;
//		desc.ArraySize = 1;
//		desc.SampleDesc.Count = 1;
//		desc.SampleDesc.Quality = 0;
//		desc.Usage = D3D11_USAGE_DEFAULT;
//		desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
//		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
//		desc.CPUAccessFlags = 0;
//		desc.MiscFlags = 0;
//
//		D3D11_SUBRESOURCE_DATA subresourceData;
//		subresourceData.pSysMem = embeddedTexture->pcData;
//		subresourceData.SysMemPitch = embeddedTexture->mWidth * 4;
//		subresourceData.SysMemSlicePitch = embeddedTexture->mWidth * embeddedTexture->mHeight * 4;
//
//		ID3D11Texture2D* texture2D = nullptr;
//		DX::ThrowIfFailed(
//			dev_->CreateTexture2D(&desc, &subresourceData, &texture2D),
//			"Failed to create a Texture2D."
//		);
//
//		DX::ThrowIfFailed(
//			dev_->CreateShaderResourceView(texture2D, nullptr, &texture),
//			"Failed to create a ShaderResourceView."
//		);
//
//		return texture;
//	}
//
//	// mHeight is 0, so try to load a compressed texture of mWidth bytes
//	const size_t size = embeddedTexture->mWidth;
//
//	DX::ThrowIfFailed(
//		CreateWICTextureFromMemory(dev_, devcon_, reinterpret_cast<const unsigned char*>(embeddedTexture->pcData), size, nullptr, &texture),
//		"WIC Texture creation failed."
//	);
//
//	return texture;
//}

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

Mesh<LoaderVertexType> processMesh(D3DCore& core, aiMesh* mesh, const aiScene* scene) {
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
	//if (mesh->mMaterialIndex >= 0) {
	//	aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

	//	std::vector<Texture> diffuseMaps = this->loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse", scene);
	//	textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
	//}

	auto newMesh = Mesh<LoaderVertexType>(core, vertices, indices);

	if (scene->mNumMaterials > 0) {
		auto mat = scene->mMaterials[mesh->mMaterialIndex];

		for (auto i = 0; i < mat->mNumProperties; i++) {
			auto prop = mat->mProperties[i];
			newMesh.materialProperties.push_back(GetPropertyFromAssimpProperty(prop));
		}
	}

	// Test
	for (auto prop : newMesh.materialProperties) {
		OutputDebugString(StringUtils::StringToWString(prop.name + ": " + prop.GetValueAsString() + "\n").c_str());
	}

	return newMesh;
}

ModelLoaderNode processNode(D3DCore& core, aiNode* node, const aiScene* scene) {
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
		newNode.meshes.push_back(processMesh(core, mesh, scene));
	}

	for (UINT i = 0; i < node->mNumChildren; i++) {
		auto newChild = processNode(core, node->mChildren[i], scene);
		newNode.children.push_back(newChild);
	}

	return newNode;
}

std::optional<ModelLoaderNode> Load(D3DCore& core, std::string filePath) {
	Assimp::Importer importer;

	const aiScene* pScene = importer.ReadFile(filePath,
		aiProcess_Triangulate |
		aiProcess_ConvertToLeftHanded);

	if (pScene == nullptr)
		return std::nullopt;

	return processNode(core, pScene->mRootNode, pScene);
}

ModelLoaderNode ModelLoader::LoadFromFile(D3DCore& core, std::string path) {
	auto mesh = Load(core, path);
	if (mesh.has_value())
		return mesh.value();
	else
		throw std::runtime_error("Failed to load model from file: " + path);
}