#include "pch.h"
#include "Material.h"

std::unordered_map<std::wstring, std::shared_ptr<URM::Core::PixelShader>> URM::Core::Material::mPixelShadersCache;

void URM::Core::Material::Bind(D3DCore& core, UINT bufferIndex)
{
	if (this->mShader == nullptr) {
		this->mShader = this->GetShader(core);
	}

	this->mShader->Bind(core);

	this->BindData(core, bufferIndex);
}

std::shared_ptr<URM::Core::PixelShader> URM::Core::Material::GetShader(D3DCore& core) {
	if (this->mShader == nullptr) {
		auto path = this->GetShaderFilePath();
		auto newShader = mPixelShadersCache.find(path);
		if (newShader != mPixelShadersCache.end()) {
			this->mShader = newShader->second;
		} else {
			this->mShader = std::shared_ptr<PixelShader>(new PixelShader(core, path));
			mPixelShadersCache[path] = this->mShader;
		}
	}

	return this->mShader;
}