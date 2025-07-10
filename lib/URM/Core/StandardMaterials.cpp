#include "pch.h"
#include "StandardMaterials.h"

std::shared_ptr<URM::Core::PixelShader> URM::Core::MaterialSimple::defaultShader = nullptr;
std::shared_ptr<URM::Core::PixelShader> URM::Core::MaterialPBR::defaultShader = nullptr;


std::shared_ptr<URM::Core::PixelShader> URM::Core::MaterialSimple::GetShader(D3DCore& core) {
	if (this->defaultShader == nullptr) {
		this->defaultShader.reset(new Core::PixelShader(core, L"SimplePixelShader.cso"));
	}

	return this->defaultShader;
}

void URM::Core::MaterialSimple::UploadData(D3DCore& core, bool useAlbedoTexture) {
	this->data.useAlbedoTexture = useAlbedoTexture ? 1 : 0;
	this->mConstantBuffer.UpdateWithData(core, &this->data);
}

URM::Core::MaterialSimple::MaterialSimple(D3DCore& core, MaterialSimpleData data) : Material(D3DConstantBuffer::Create<URM::Core::MaterialSimpleData>(core, PIXEL)), data(data) {}

std::shared_ptr<URM::Core::PixelShader> URM::Core::MaterialPBR::GetShader(D3DCore& core) {
	if (this->defaultShader == nullptr) {
		this->defaultShader.reset(new Core::PixelShader(core, L"PixelPBRShader.cso"));
	}

	return this->defaultShader;
}

void URM::Core::MaterialPBR::UploadData(D3DCore& core, bool useAlbedoTexture) {
	this->data.useAlbedoTexture = useAlbedoTexture ? 1 : 0;
	this->mConstantBuffer.UpdateWithData(core, &this->data);
}

URM::Core::MaterialPBR::MaterialPBR(D3DCore& core, MaterialPBRData data) : Material(D3DConstantBuffer::Create<URM::Core::MaterialPBRData>(core, PIXEL)), data(data) {}
