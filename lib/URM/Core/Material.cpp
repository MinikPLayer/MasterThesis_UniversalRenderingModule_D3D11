#include "pch.h"
#include "Material.h"

std::shared_ptr<URM::Core::PixelShader> URM::Core::MaterialSimple::defaultShader = nullptr;
std::shared_ptr<URM::Core::PixelShader> URM::Core::MaterialPBR::defaultShader = nullptr;

void URM::Core::Material::Bind(D3DCore& core, UINT bufferIndex)
{
	if (this->mShader == nullptr) {
		this->mShader = this->GetShader(core);
	}

	this->mShader->Bind(core);
	this->mConstantBuffer.Bind(core, bufferIndex);
}

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

URM::Core::MaterialSimple::MaterialSimple(D3DCore& core, MaterialSimple::Data data) : Material(D3DConstantBuffer::Create<URM::Core::MaterialSimple::Data>(core, PIXEL)), data(data) {}

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

URM::Core::MaterialPBR::MaterialPBR(D3DCore& core, MaterialPBR::Data data) : Material(D3DConstantBuffer::Create<URM::Core::MaterialSimple::Data>(core, PIXEL)), data(data) {}
