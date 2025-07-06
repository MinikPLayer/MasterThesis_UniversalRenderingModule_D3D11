#include "pch.h"
#include "D3DCore.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")

using namespace DirectX;

namespace URM::Core {
	void D3DCore::CreateDevice() {
		UINT creationFlags = 0;

#if !NDEBUG
		creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

		static constexpr D3D_FEATURE_LEVEL allowedFeatureLevels[] = {D3D_FEATURE_LEVEL_11_1};

		ComPtr<ID3D11Device> newDevice;
		ComPtr<ID3D11DeviceContext> newContext;

		DX::ThrowIfFailed(
			D3D11CreateDevice(
				nullptr,
				D3D_DRIVER_TYPE_HARDWARE,
				nullptr,
				creationFlags,
				allowedFeatureLevels,
				std::size(allowedFeatureLevels),
				D3D11_SDK_VERSION,
				newDevice.ReleaseAndGetAddressOf(),
				&this->mFeatureLevel,
				newContext.ReleaseAndGetAddressOf()
			)
		);

#if !NDEBUG
		ComPtr<ID3D11Debug> d3dDebug;
		if (SUCCEEDED(newDevice.As(&d3dDebug))) {
			ComPtr<ID3D11InfoQueue> d3dInfoQueue;
			if (SUCCEEDED(d3dDebug.As(&d3dInfoQueue))) {
				d3dInfoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_CORRUPTION, true);
				d3dInfoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_ERROR, true);

				D3D11_MESSAGE_ID hide[] =
				{
					D3D11_MESSAGE_ID_SETPRIVATEDATA_CHANGINGPARAMS,
				};
				D3D11_INFO_QUEUE_FILTER filter = {};
				filter.DenyList.NumIDs = static_cast<UINT>(std::size(hide));
				filter.DenyList.pIDList = hide;
				d3dInfoQueue->AddStorageFilterEntries(&filter);
			}
		}
#endif

		DX::ThrowIfFailed(newDevice.As(&this->mDevice));
		DX::ThrowIfFailed(newContext.As(&this->mContext));
	}

	void D3DCore::CreateResources() {
		// ClearFramebuffer the previous window size specific context.
		this->mContext->OMSetRenderTargets(0, nullptr, nullptr);
		this->mRenderTargetView.Reset();
		this->mDepthStencilView.Reset();
		this->mContext->Flush();

		auto windowSize = this->mWindow->GetSize();

		const UINT backBufferWidth = static_cast<UINT>(windowSize.width);
		const UINT backBufferHeight = static_cast<UINT>(windowSize.height);
		constexpr DXGI_FORMAT backBufferFormat = DXGI_FORMAT_B8G8R8A8_UNORM;
		constexpr DXGI_FORMAT depthBufferFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
		constexpr UINT backBufferCount = 2;

		// If the swap chain already exists, resize it, otherwise create one.
		if (this->mSwapChain) {
			HRESULT hr = this->mSwapChain->ResizeBuffers(backBufferCount, backBufferWidth, backBufferHeight, backBufferFormat, DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING);

			if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET) {
				// If the device was removed for any reason, a new device and swap chain will need to be created.
				OnDeviceLost();

				// Everything is set up now. Do not continue execution of this method. OnDeviceLost will reenter this method 
				// and correctly set up the new device.
				return;
			}
			DX::ThrowIfFailed(hr);
		}
		else {
			// First, retrieve the underlying DXGI Device from the D3D Device.
			ComPtr<IDXGIDevice1> dxgiDevice;
			DX::ThrowIfFailed(this->mDevice.As(&dxgiDevice));

			// Identify the physical adapter (GPU or card) this device is running on.
			ComPtr<IDXGIAdapter> dxgiAdapter;
			DX::ThrowIfFailed(dxgiDevice->GetAdapter(dxgiAdapter.GetAddressOf()));

			// And obtain the factory object that created it.
			ComPtr<IDXGIFactory2> dxgiFactory;
			DX::ThrowIfFailed(dxgiAdapter->GetParent(IID_PPV_ARGS(dxgiFactory.GetAddressOf())));

			// Create a descriptor for the swap chain.
			DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
			swapChainDesc.Width = backBufferWidth;
			swapChainDesc.Height = backBufferHeight;
			swapChainDesc.Format = backBufferFormat;
			swapChainDesc.SampleDesc.Count = 1;
			swapChainDesc.SampleDesc.Quality = 0;
			swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
			swapChainDesc.BufferCount = backBufferCount;
			swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
			swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;

			DXGI_SWAP_CHAIN_FULLSCREEN_DESC fsSwapChainDesc = {};
			fsSwapChainDesc.Windowed = TRUE;

			// Create a SwapChain from a Win32 window.
			DX::ThrowIfFailed(dxgiFactory->CreateSwapChainForHwnd(
				this->mDevice.Get(),
				this->mWindow->GetHandle(),
				&swapChainDesc,
				&fsSwapChainDesc,
				nullptr,
				this->mSwapChain.ReleaseAndGetAddressOf()
			));

			// This template does not support exclusive fullscreen mode and prevents DXGI from responding to the ALT+ENTER shortcut.
			DX::ThrowIfFailed(dxgiFactory->MakeWindowAssociation(this->mWindow->GetHandle(), DXGI_MWA_NO_ALT_ENTER));
		}

		// Obtain the backbuffer for this window which will be the final 3D rendertarget.
		ComPtr<ID3D11Texture2D> backBuffer;
		DX::ThrowIfFailed(this->mSwapChain->GetBuffer(0, IID_PPV_ARGS(backBuffer.GetAddressOf())));

		// Create a view interface on the rendertarget to use on bind.
		DX::ThrowIfFailed(this->mDevice->CreateRenderTargetView(backBuffer.Get(), nullptr, this->mRenderTargetView.ReleaseAndGetAddressOf()));

		// Allocate a 2-D surface as the depth/stencil buffer and
		// create a DepthStencil view on this surface to use on bind.
		CD3D11_TEXTURE2D_DESC depthStencilDesc(depthBufferFormat, backBufferWidth, backBufferHeight, 1, 1, D3D11_BIND_DEPTH_STENCIL);

		ComPtr<ID3D11Texture2D> depthStencil;
		DX::ThrowIfFailed(this->mDevice->CreateTexture2D(&depthStencilDesc, nullptr, depthStencil.GetAddressOf()));

		DX::ThrowIfFailed(this->mDevice->CreateDepthStencilView(depthStencil.Get(), nullptr, this->mDepthStencilView.ReleaseAndGetAddressOf()));
	}

	void D3DCore::OnDeviceLost() {
		this->FreeResources();

		this->CreateDevice();
		this->CreateResources();
	}

	void D3DCore::FreeResources() {
		this->mDepthStencilView.Reset();
		this->mRenderTargetView.Reset();
		this->mSwapChain.Reset();
		this->mContext.Reset();
		this->mDevice.Reset();
	}

	void D3DCore::WindowResized(const Window& win, Size2i oldSize, Size2i newSize) {
		if (oldSize == newSize) {
			return;
		}

		auto windowSize = win.GetSize();
		spdlog::info("Window resized from {}x{} to {}x{} [{}x{}]", oldSize.width, oldSize.height, newSize.width, newSize.height, windowSize.width, windowSize.height);

		CreateResources();

		if (this->onWindowResized)
			this->onWindowResized(*this, oldSize, newSize);
	}

	void D3DCore::WindowFocusChanged(Window&, bool isFocused) {
		if (this->onWindowFocusChanged)
			this->onWindowFocusChanged(*this, isFocused);
	}

	bool D3DCore::WindowCloseRequested(Window&) {
		if (this->onWindowCloseRequested) {
			return this->onWindowCloseRequested(*this);
		}

		return false;
	}

	void D3DCore::WindowPaint(Window&) {
		if (this->onWindowPaint) {
			this->onWindowPaint(*this);
		}
	}

	void D3DCore::SetPrimitiveTopology(PrimitiveTopologies topology) const {
		mContext->IASetPrimitiveTopology(static_cast<D3D11_PRIMITIVE_TOPOLOGY>(topology));
	}

	void D3DCore::ClearFramebuffer(Color color) {
		// ClearFramebuffer the views.
		mContext->ClearRenderTargetView(this->mRenderTargetView.Get(), color);
		mContext->ClearDepthStencilView(this->mDepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

		mContext->OMSetRenderTargets(1, this->mRenderTargetView.GetAddressOf(), this->mDepthStencilView.Get());

		// Set the viewport.
		auto size = GetWindow().GetSize();
		D3D11_VIEWPORT viewport = {0.0f, 0.0f, static_cast<float>(size.width), static_cast<float>(size.height), 0.f, 1.f};
		mContext->RSSetViewports(1, &viewport);
	}

	void D3DCore::Present(const int syncInterval) {
		UINT flags = 0;
		if (syncInterval == 0) {
			flags |= DXGI_PRESENT_ALLOW_TEARING;
		}
		HRESULT hr = this->mSwapChain->Present(syncInterval, flags);

		// If the device was reset we must completely reinitialize the renderer.
		if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET) {
			OnDeviceLost();
		}
		else {
			DX::ThrowIfFailed(hr);
		}
	}

	D3DCore::D3DCore(WindowCreationParams windowParams) {
		this->mWindow = std::make_unique<Window>(windowParams);

		this->CreateDevice();
		this->CreateResources();

		this->mWindow->OnResize = std::bind(&D3DCore::WindowResized, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
		this->mWindow->OnFocusChange = std::bind(&D3DCore::WindowFocusChanged, this, std::placeholders::_1, std::placeholders::_2);
		this->mWindow->OnCloseRequested = std::bind(&D3DCore::WindowCloseRequested, this, std::placeholders::_1);
		this->mWindow->OnPaint = std::bind(&D3DCore::WindowPaint, this, std::placeholders::_1);
	}
}
