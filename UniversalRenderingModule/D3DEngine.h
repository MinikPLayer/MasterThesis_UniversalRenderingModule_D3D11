#pragma once

#include "Window.h"
#include <functional>

using Microsoft::WRL::ComPtr;

class D3DEngine {
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
	std::function<void(D3DEngine& engine, Size2i oldSize, Size2i newSize)> OnWindowResized = {};
	std::function<void(D3DEngine& engine, bool isFocused)> OnWindowFocusChanged = {};
	std::function<void(D3DEngine& engine)> OnWindowPaint = {};

	// Return true to prevent closing.
	std::function<bool(D3DEngine& engine)> OnWindowCloseRequested = {};

	ID3D11Device1* GetDevice() { return this->device.Get(); }
	ID3D11DeviceContext1* GetContext() { return this->context.Get(); }
	IDXGISwapChain1* GetSwapChain() { return this->swapChain.Get(); }
	ID3D11RenderTargetView* GetRenderTargetView() { return this->renderTargetView.Get(); }
	ID3D11DepthStencilView* GetDepthStencilView() { return this->depthStencilView.Get(); }

	Window& GetWindow() { return *this->window; }

	void Clear(DirectX::XMVECTORF32 color);
	void Present(int syncInterval);

	D3DEngine(const D3DEngine&) = delete;
	D3DEngine& operator= (const D3DEngine&) = delete;

	D3DEngine(WindowCreationParams windowParams);
};

