#include "pch.h"
#include "ModelLoader.h"
#include "D3DTexture2D.h"
#include "Mesh.h"
#include "MaterialProperty.h"

#include <assimp/mesh.h>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

namespace URM::Core {
	D3DTexture2D LoadEmbeddedTexture(const D3DCore& core, const std::string& type, const aiTexture* embeddedTexture) {
		auto width = embeddedTexture->mWidth;
		auto height = embeddedTexture->mHeight;
		auto pixelData = embeddedTexture->pcData;

		return D3DTexture2D::CreateFromMemory(core, type, Size2i(width, height), reinterpret_cast<Texel2D*>(pixelData));
	}

	std::vector<D3DTexture2D> LoadMaterialTextures(const D3DCore& core, std::map<std::string, D3DTexture2D>& loadedTexturesPool, const std::string& fileDirectory, const aiMaterial* mat, aiTextureType type, const std::string& typeName, const aiScene* scene) {
		std::vector<D3DTexture2D> textures;
		for (UINT i = 0; i < mat->GetTextureCount(type); i++) {
			aiString str;
			mat->GetTexture(type, i, &str);
			// Check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
			if(loadedTexturesPool.count(str.C_Str()) > 0) {
				spdlog::trace("[TextureLoad()] Reusing cached texture: {}", str.C_Str());
				textures.push_back(loadedTexturesPool.at(str.C_Str()));
			}
			else {
				spdlog::trace("[TextureLoad()] Loading texture from file: {}", str.C_Str());

				const aiTexture* embeddedTexture = scene->GetEmbeddedTexture(str.C_Str());

				D3DTexture2D texture = (embeddedTexture == nullptr) ? D3DTexture2D::CreateFromFile(core, fileDirectory + '/' + std::string(str.C_Str()), typeName) : LoadEmbeddedTexture(core, typeName, embeddedTexture);

				textures.push_back(texture);
				loadedTexturesPool.insert({ str.C_Str(), texture });  // Store it as texture loaded for entire model, to ensure we won't unnecessarily load duplicate textures.
			}
		}
		return textures;
	}


	float UnpackFloat(const uint8_t* b) {
		uint32_t temp = ((static_cast<uint32_t>(b[3]) << 24) |
		        (static_cast<uint32_t>(b[2]) << 16) |
		        (static_cast<uint32_t>(b[1]) << 8) |
		        static_cast<uint32_t>(b[0]));
		return *reinterpret_cast<float*>(&temp);
	}

	double UnpackDouble(const uint8_t* b) {
		uint64_t temp = (static_cast<uint64_t>(b[7]) << 56) |
		       (static_cast<uint64_t>(b[6]) << 48) |
		       (static_cast<uint64_t>(b[5]) << 40) |
		       (static_cast<uint64_t>(b[4]) << 32) |
		       (static_cast<uint64_t>(b[3]) << 24) |
		       (static_cast<uint64_t>(b[2]) << 16) |
		       (static_cast<uint64_t>(b[1]) << 8) |
		       b[0];
		return *reinterpret_cast<double*>(&temp);
	}

	int UnpackInteger(const uint8_t* b) {
		int temp = ((b[3] << 24) |
		            (b[2] << 16) |
		            (b[1] << 8) |
		            b[0]);
		return temp;
	}

	template<typename T, int TSizeInBytes>
	std::vector<T> UnpackPropertyData(const unsigned char* buf, unsigned int bufSizeInBytes, std::function<T(const unsigned char*)> unpackFunc) {

		std::vector<T> temp;
		for (size_t offset = 0; offset < bufSizeInBytes; offset += TSizeInBytes) {
			auto value = unpackFunc(buf + offset);
			temp.push_back(value);
		}
		return temp;
	}

	MaterialProperty GetPropertyFromAssimpProperty(const aiMaterialProperty* prop) {
		auto nameString = std::string(prop->mKey.C_Str());

		switch (prop->mType) {
			case aiPTI_String:
				// Skip first 4 bytes, which are the length of the string + 1 byte for the null terminator
				// [TEST]: Verify prop->mDataLength vs first 4 bytes
				return MaterialProperty::CreateString(nameString, prop->mData + 4, prop->mDataLength - 4 - 1);

			case aiPTI_Float:
				return MaterialProperty::CreateFloat(nameString, UnpackPropertyData<float, 4>(reinterpret_cast<const uint8_t*>(prop->mData), prop->mDataLength, UnpackFloat));

			case aiPTI_Double:
				return MaterialProperty::CreateDouble(nameString, UnpackPropertyData<double, 8>(reinterpret_cast<const uint8_t*>(prop->mData), prop->mDataLength, UnpackDouble));

			case aiPTI_Integer:
				return MaterialProperty::CreateInteger(nameString, UnpackPropertyData<int, 4>(reinterpret_cast<const uint8_t*>(prop->mData), prop->mDataLength, UnpackInteger));

			case aiPTI_Buffer: {
				auto newData = new unsigned char[prop->mDataLength];
				std::copy_n(prop->mData, prop->mDataLength, newData);
				return MaterialProperty::CreateBuffer(nameString, newData, prop->mDataLength);
			}

			default:
				throw std::runtime_error("Unknown property type");
		}
	}

	std::shared_ptr<Mesh<ModelLoaderVertexType>> ProcessMesh(D3DCore& core, std::map<std::string, D3DTexture2D>& loadedTexturesPool, const std::string& fileDirectory, const aiMesh* mesh, const aiScene* scene) {
		// Data to fill
		std::vector<ModelLoaderVertexType> vertices;
		std::vector<UINT> indices;

		// Walk through each of the mesh's vertices
		for (UINT i = 0; i < mesh->mNumVertices; i++) {
			ModelLoaderVertexType vertex;

			vertex.position.x = mesh->mVertices[i].x;
			vertex.position.y = mesh->mVertices[i].y;
			vertex.position.z = mesh->mVertices[i].z;

			if (mesh->mTextureCoords[0]) {
				vertex.texcoord.x = mesh->mTextureCoords[0][i].x;
				vertex.texcoord.y = mesh->mTextureCoords[0][i].y;
			}

			if (mesh->HasNormals()) {
				vertex.normal.x = mesh->mNormals[i].x;
				vertex.normal.y = mesh->mNormals[i].y;
				vertex.normal.z = mesh->mNormals[i].z;
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

		auto newMesh = new Mesh<ModelLoaderVertexType>(core, vertices, indices, textures);
		newMesh->materialProperties = materialProperties;

		return std::shared_ptr<Mesh<ModelLoaderVertexType>>(newMesh);
	}

	std::shared_ptr<ModelLoaderNode> ProcessNode(D3DCore& core, std::map<std::string, D3DTexture2D>& loadedTexturesPool, const std::string& fileDirectory, const aiNode* node, const aiScene* scene) {
		auto newNode = std::make_shared<ModelLoaderNode>();
		if (node->mTransformation.IsIdentity()) {
			newNode->transform = DirectX::XMMatrixIdentity();
		}
		else {
			aiMatrix4x4 transform = node->mTransformation;
			newNode->transform = XMMatrixTranspose(XMLoadFloat4x4(reinterpret_cast<DirectX::XMFLOAT4X4*>(&transform)));
		}

		for (UINT i = 0; i < node->mNumMeshes; i++) {
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			newNode->meshes.push_back(std::shared_ptr(ProcessMesh(core, loadedTexturesPool, fileDirectory, mesh, scene)));
		}

		for (UINT i = 0; i < node->mNumChildren; i++) {
			auto&& newChild = ProcessNode(core, loadedTexturesPool, fileDirectory, node->mChildren[i], scene);
			newNode->children.push_back(newChild);
		}

		return std::move(newNode);
	}

	std::shared_ptr<ModelLoaderNode> Load(D3DCore& core, std::map<std::string, D3DTexture2D>& loadedTexturesPool, const std::string& filePath) {
		Assimp::Importer importer;

		const aiScene* pScene = importer.ReadFile(filePath, aiProcess_Triangulate);

		if (pScene == nullptr)
			return nullptr;

		auto dir = StringUtils::GetDirectoryFromPath(filePath);
		return ProcessNode(core, loadedTexturesPool, dir, pScene->mRootNode, pScene);
	}

	std::shared_ptr<ModelLoaderNode> ModelLoader::LoadFromFile(D3DCore& core, std::map<std::string, D3DTexture2D>& loadedTexturesPool, const std::string& path) {
		auto mesh = Load(core, loadedTexturesPool, path);
		if (mesh != nullptr) {
			return mesh;
		}
		throw std::runtime_error("Failed to load model from file: " + path);
	}
}
