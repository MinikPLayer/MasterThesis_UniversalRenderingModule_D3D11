#include "pch.h"
#include "D3DTexture2D.h"
#include <directxtk/WICTextureLoader.h>

namespace URM::Core {
	std::string D3DTexture2D::GetPath() {
		return mPath;
	}

	void D3DTexture2D::Bind(const D3DCore& core, UINT slot) {
		core.GetContext()->PSSetShaderResources(slot, 1, this->mTextureView.GetAddressOf());
	}

	D3DTexture2D::D3DTexture2D(const D3DCore& core, const std::string& name, const std::string& type, Size2i size, const Texel2D* pixelData, const D3DTexture2DCreationParams& params) {
		this->mPath = name;
		this->mType = type;

		// mHeight is 0, so try to load a compressed texture of mWidth bytes
		if (size.height == 0) {
			const size_t bufferLength = size.width;

			DX::ThrowIfFailed(
				DirectX::CreateWICTextureFromMemory(core.GetDevice(), core.GetContext(), reinterpret_cast<const unsigned char*>(pixelData), bufferLength, nullptr, this->mTextureView.GetAddressOf()),
				"WIC Texture creation failed."
			);
		}
		// Load an uncompressed ARGB8888 embedded texture
		else {
			D3D11_TEXTURE2D_DESC desc;
			desc.Width = size.width;
			desc.Height = size.height;
			desc.MipLevels = params.mipLevels;
			desc.ArraySize = params.arraySize;
			desc.SampleDesc.Count = params.samplesCount;
			desc.SampleDesc.Quality = params.samplesQuality;
			desc.Usage = params.usage;
			desc.Format = params.format;
			desc.BindFlags = params.bindFlags;
			desc.CPUAccessFlags = params.cpuAccessFlags;
			desc.MiscFlags = params.miscFlags;

			D3D11_SUBRESOURCE_DATA subresourceData;
			subresourceData.pSysMem = pixelData;
			subresourceData.SysMemPitch = size.width * 4;
			subresourceData.SysMemSlicePitch = size.width * size.height * 4;

			ID3D11Texture2D* texture2D;
			DX::ThrowIfFailed(
				core.GetDevice()->CreateTexture2D(&desc, &subresourceData, &texture2D),
				"Failed to create a Texture2D."
			);

			DX::ThrowIfFailed(
				core.GetDevice()->CreateShaderResourceView(texture2D, nullptr, this->mTextureView.GetAddressOf()),
				"Failed to create a Texture2D ShaderResourceView."
			);
		}
	}

	D3DTexture2D::D3DTexture2D(const D3DCore& core, const std::string& path, const std::string& type, const D3DTexture2DCreationParams& params) {
		this->mPath = path;
		this->mType = type;
		// Load a compressed texture
		std::wstring widePath = StringUtils::StringToWString(path);

		DX::ThrowIfFailed(
			CreateWICTextureFromFileEx(
				core.GetDevice(),
				core.GetContext(),
				widePath.c_str(),
				0,
				params.usage,
				params.bindFlags,
				params.cpuAccessFlags,
				params.miscFlags,
				DirectX::WIC_LOADER_DEFAULT,
				nullptr,
				this->mTextureView.GetAddressOf()
			),
			"Failed to create a Texture2D from file (" + path + ")."
		);
	}
}
