#include "pch.h"
#include "Material.h"

void URM::Core::Material::Bind(D3DCore& core, UINT bufferIndex)
{
	if (this->mShader == nullptr) {
		this->mShader = this->GetShader(core);
	}

	this->mShader->Bind(core);
	this->mConstantBuffer.Bind(core, bufferIndex);
}