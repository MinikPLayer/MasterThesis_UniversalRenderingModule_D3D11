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
public:
	int Run();
	Window& GetWindow() {
		return *window;
	}

	D3DEngine(WindowCreationParams windowParams);
};

