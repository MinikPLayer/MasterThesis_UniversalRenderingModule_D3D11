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

	Window& GetWindow() {
		return *this->window;
	}

	D3DEngine(WindowCreationParams windowParams);
};

