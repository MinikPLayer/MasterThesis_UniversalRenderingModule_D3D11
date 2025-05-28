#pragma once

#include <d3d11.h>
#include "Window.h"
#include "Utils.h"
#include <functional>

using Microsoft::WRL::ComPtr;

namespace URM::Core {
	enum class PrimitiveTopologies {
		POINT_LIST     = D3D11_PRIMITIVE_TOPOLOGY_POINTLIST,
		LINE_LIST      = D3D11_PRIMITIVE_TOPOLOGY_LINELIST,
		LINE_STRIP     = D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP,
		TRIANGLE_LIST  = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
		TRIANGLE_STRIP = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP,

		LINELIST_ADJ      = D3D11_PRIMITIVE_TOPOLOGY_LINELIST_ADJ,
		LINESTRIP_ADJ     = D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP_ADJ,
		TRIANGLELIST_ADJ  = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST_ADJ,
		TRIANGLESTRIP_ADJ = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP_ADJ,
	};

	class D3DCore : NonCopyable {
		std::unique_ptr<Window> mWindow;

		D3D_FEATURE_LEVEL mFeatureLevel;

		ComPtr<ID3D11Device1> mDevice;
		ComPtr<ID3D11DeviceContext1> mContext;

		ComPtr<ID3D11RenderTargetView> mRenderTargetView;
		ComPtr<IDXGISwapChain1> mSwapChain;
		ComPtr<ID3D11DepthStencilView> mDepthStencilView;

		void CreateDevice();

		// Create or recreate resources.
		void CreateResources();

		void OnDeviceLost();
		void FreeResources();

		void WindowResized(Window& window, Size2i oldSize, Size2i newSize);
		void WindowFocusChanged(Window& window, bool isFocused);
		bool WindowCloseRequested(Window& window);
		void WindowPaint(Window& window);
	public:
		std::function<void(D3DCore& core, Size2i oldSize, Size2i newSize)> onWindowResized = {};
		std::function<void(D3DCore& core, bool isFocused)> onWindowFocusChanged = {};
		std::function<void(D3DCore& core)> onWindowPaint = {};

		// Return true to prevent closing.
		std::function<bool(D3DCore& core)> onWindowCloseRequested = {};

		ID3D11Device1* GetDevice() const { return this->mDevice.Get(); }
		ID3D11DeviceContext1* GetContext() const { return this->mContext.Get(); }
		IDXGISwapChain1* GetSwapChain() const { return this->mSwapChain.Get(); }
		ID3D11RenderTargetView* GetRenderTargetView() const { return this->mRenderTargetView.Get(); }
		ID3D11DepthStencilView* GetDepthStencilView() const { return this->mDepthStencilView.Get(); }

		Window& GetWindow() const { return *this->mWindow; }

		void SetPrimitiveTopology(PrimitiveTopologies topology) const;
		void Clear(Color color);
		void Present(int syncInterval);

		D3DCore(const D3DCore&) = delete;
		D3DCore& operator=(const D3DCore&) = delete;

		D3DCore(WindowCreationParams windowParams);
	};
}
