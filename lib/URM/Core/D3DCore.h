#pragma once

#include <d3d11.h>
#include "Window.h"
#include "Utils.h"
#include <functional>

using Microsoft::WRL::ComPtr;

enum class PrimitiveTopologies {
	POINT_LIST = D3D11_PRIMITIVE_TOPOLOGY_POINTLIST,
	LINE_LIST = D3D11_PRIMITIVE_TOPOLOGY_LINELIST,
	LINE_STRIP = D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP,
	TRIANGLE_LIST = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
	TRIANGLE_STRIP = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP,

	LINELIST_ADJ = D3D11_PRIMITIVE_TOPOLOGY_LINELIST_ADJ,
	LINESTRIP_ADJ = D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP_ADJ,
	TRIANGLELIST_ADJ = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST_ADJ,
	TRIANGLESTRIP_ADJ = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP_ADJ,
};

class D3DCore {
	std::unique_ptr<Window> window;

	D3D_FEATURE_LEVEL featureLevel;

	ComPtr<ID3D11Device1> device;
	ComPtr<ID3D11DeviceContext1> context;

	ComPtr<ID3D11RenderTargetView> renderTargetView;
	ComPtr<IDXGISwapChain1> swapChain;
	ComPtr<ID3D11DepthStencilView> depthStencilView;

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
	std::function<void(D3DCore& core, Size2i oldSize, Size2i newSize)> OnWindowResized = {};
	std::function<void(D3DCore& core, bool isFocused)> OnWindowFocusChanged = {};
	std::function<void(D3DCore& core)> OnWindowPaint = {};

	// Return true to prevent closing.
	std::function<bool(D3DCore& core)> OnWindowCloseRequested = {};

	ID3D11Device1* GetDevice() { return this->device.Get(); }
	ID3D11DeviceContext1* GetContext() { return this->context.Get(); }
	IDXGISwapChain1* GetSwapChain() { return this->swapChain.Get(); }
	ID3D11RenderTargetView* GetRenderTargetView() { return this->renderTargetView.Get(); }
	ID3D11DepthStencilView* GetDepthStencilView() { return this->depthStencilView.Get(); }

	Window& GetWindow() { return *this->window; }

	void SetPrimitiveTopology(PrimitiveTopologies topology);
	void Clear(DirectX::XMVECTORF32 color);
	void Present(int syncInterval);

	D3DCore(const D3DCore&) = delete;
	D3DCore& operator= (const D3DCore&) = delete;

	D3DCore(WindowCreationParams windowParams);
};

