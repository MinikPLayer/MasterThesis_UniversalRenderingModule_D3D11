#include "pch.h"
#include "StandardMaterials.h"

const wchar_t* URM::Core::MaterialSimple::GetShaderFilePath() const {
	return L"SimplePixelShader.cso";
}

void URM::Core::MaterialSimple::UploadData(D3DCore& core, bool useAlbedoTexture) {
	this->data.useAlbedoTexture = useAlbedoTexture ? 1 : 0;
	this->mConstantBuffer.UpdateWithData(core, &this->data);
}

URM::Core::MaterialSimple::MaterialSimple(D3DCore& core, MaterialSimpleData data) : MaterialWithData(core, data) {}

const wchar_t* URM::Core::MaterialPBR::GetShaderFilePath() const {
	return L"PixelPBRShader.cso";
}

void URM::Core::MaterialPBR::UploadData(D3DCore& core, bool useAlbedoTexture) {
	this->data.useAlbedoTexture = useAlbedoTexture ? 1 : 0;
	this->mConstantBuffer.UpdateWithData(core, &this->data);
}

URM::Core::MaterialPBR::MaterialPBR(D3DCore& core, MaterialPBRData data) : MaterialWithData(core, data) {}
