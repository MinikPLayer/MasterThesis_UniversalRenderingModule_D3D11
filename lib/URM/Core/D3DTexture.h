#pragma once

#include <pch.h>

using namespace Microsoft::WRL;

class D3DTexture {
	ComPtr<ID3D11ShaderResourceView> textureView;
};