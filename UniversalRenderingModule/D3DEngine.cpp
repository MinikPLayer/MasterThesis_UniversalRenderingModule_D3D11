#include "pch.h"
#include "D3DEngine.h"

void D3DEngine::CreateDevice() {
	UINT creationFlags = 0;

#if !NDEBUG
	creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif 

	static const D3D_FEATURE_LEVEL allowedFeatureLevels[] = { D3D_FEATURE_LEVEL_11_0 };

	ComPtr<ID3D11Device> newDevice;
	ComPtr<ID3D11DeviceContext> newContext;

	DX::ThrowIfFailed(
		D3D11CreateDevice(
			nullptr,
			D3D_DRIVER_TYPE_HARDWARE,
			nullptr,
			creationFlags,
			allowedFeatureLevels,
			static_cast<UINT>(std::size(allowedFeatureLevels)),
			D3D11_SDK_VERSION,
            newDevice.ReleaseAndGetAddressOf(),
			&this->featureLevel,
            newContext.ReleaseAndGetAddressOf()
		)
	);

#if !NDEBUG
	ComPtr<ID3D11Debug> d3dDebug;
	if (SUCCEEDED(newDevice.As(&d3dDebug)))
	{
		ComPtr<ID3D11InfoQueue> d3dInfoQueue;
		if (SUCCEEDED(d3dDebug.As(&d3dInfoQueue)))
		{
			d3dInfoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_CORRUPTION, true);
			d3dInfoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_ERROR, true);

			D3D11_MESSAGE_ID hide[] =
			{
				D3D11_MESSAGE_ID_SETPRIVATEDATA_CHANGINGPARAMS,
				// TODO: Add more message IDs here as needed.
			};
			D3D11_INFO_QUEUE_FILTER filter = {};
			filter.DenyList.NumIDs = static_cast<UINT>(std::size(hide));
			filter.DenyList.pIDList = hide;
			d3dInfoQueue->AddStorageFilterEntries(&filter);
		}
	}
#endif

	DX::ThrowIfFailed(newDevice.As(&this->device));
	DX::ThrowIfFailed(newContext.As(&this->context));
}

void D3DEngine::CreateResources() {
    // Clear the previous window size specific context.
    this->context->OMSetRenderTargets(0, nullptr, nullptr);
    this->renderTargetView.Reset();
    this->depthStencilView.Reset();
    this->context->Flush();

	auto windowSize = this->window->GetSize();

    const UINT backBufferWidth = static_cast<UINT>(windowSize.width);
    const UINT backBufferHeight = static_cast<UINT>(windowSize.height);
    const DXGI_FORMAT backBufferFormat = DXGI_FORMAT_B8G8R8A8_UNORM;
    const DXGI_FORMAT depthBufferFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
    constexpr UINT backBufferCount = 3;

    // If the swap chain already exists, resize it, otherwise create one.
    if (this->swapChain)
    {
        HRESULT hr = this->swapChain->ResizeBuffers(backBufferCount, backBufferWidth, backBufferHeight, backBufferFormat, 0);

        if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
        {
            // If the device was removed for any reason, a new device and swap chain will need to be created.
            OnDeviceLost();

            // Everything is set up now. Do not continue execution of this method. OnDeviceLost will reenter this method 
            // and correctly set up the new device.
            return;
        }
        else
        {
            DX::ThrowIfFailed(hr);
        }
    }
    else
    {
        // First, retrieve the underlying DXGI Device from the D3D Device.
        ComPtr<IDXGIDevice1> dxgiDevice;
        DX::ThrowIfFailed(this->device.As(&dxgiDevice));

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

        DXGI_SWAP_CHAIN_FULLSCREEN_DESC fsSwapChainDesc = {};
        fsSwapChainDesc.Windowed = TRUE;

        // Create a SwapChain from a Win32 window.
        DX::ThrowIfFailed(dxgiFactory->CreateSwapChainForHwnd(
            this->device.Get(),
            this->window->GetHandle(),
            &swapChainDesc,
            &fsSwapChainDesc,
            nullptr,
            this->swapChain.ReleaseAndGetAddressOf()
        ));

        // This template does not support exclusive fullscreen mode and prevents DXGI from responding to the ALT+ENTER shortcut.
        DX::ThrowIfFailed(dxgiFactory->MakeWindowAssociation(this->window->GetHandle(), DXGI_MWA_NO_ALT_ENTER));
    }

    // Obtain the backbuffer for this window which will be the final 3D rendertarget.
    ComPtr<ID3D11Texture2D> backBuffer;
    DX::ThrowIfFailed(this->swapChain->GetBuffer(0, IID_PPV_ARGS(backBuffer.GetAddressOf())));

    // Create a view interface on the rendertarget to use on bind.
    DX::ThrowIfFailed(this->device->CreateRenderTargetView(backBuffer.Get(), nullptr, this->renderTargetView.ReleaseAndGetAddressOf()));

    // Allocate a 2-D surface as the depth/stencil buffer and
    // create a DepthStencil view on this surface to use on bind.
    CD3D11_TEXTURE2D_DESC depthStencilDesc(depthBufferFormat, backBufferWidth, backBufferHeight, 1, 1, D3D11_BIND_DEPTH_STENCIL);

    ComPtr<ID3D11Texture2D> depthStencil;
    DX::ThrowIfFailed(this->device->CreateTexture2D(&depthStencilDesc, nullptr, depthStencil.GetAddressOf()));

    DX::ThrowIfFailed(this->device->CreateDepthStencilView(depthStencil.Get(), nullptr, this->depthStencilView.ReleaseAndGetAddressOf()));

    // TODO: Initialize windows-size dependent objects here.
}

void D3DEngine::OnDeviceLost() {
    this->FreeResources();

    this->CreateDevice();
    this->CreateResources();
}

void D3DEngine::FreeResources() {
    this->depthStencilView.Reset();
    this->renderTargetView.Reset();
    this->swapChain.Reset();
    this->context.Reset();
    this->device.Reset();
}

void D3DEngine::WindowResized(Window& window, Size2i oldSize, Size2i newSize) {
	spdlog::info("Window resized from {}x{} to {}x{}", oldSize.width, oldSize.height, newSize.width, newSize.height);

	if (this->OnWindowResized)
		this->OnWindowResized(*this, oldSize, newSize);
}

void D3DEngine::WindowFocusChanged(Window& window, bool isFocused) {
	if (this->OnWindowFocusChanged)
		this->OnWindowFocusChanged(*this, isFocused);
}

bool D3DEngine::WindowClosing(Window& window) {
    if (this->OnWindowClosing) {
		return this->OnWindowClosing(*this);
    }

    return false;
}

void D3DEngine::WindowPaint(Window& window) {
    if (this->OnWindowPaint) {
		this->OnWindowPaint(*this);
    }
}

int D3DEngine::Run() {
    return this->window->RunHandlerLoop();
}

D3DEngine::D3DEngine(WindowCreationParams windowParams) {
	this->window = std::make_unique<Window>(windowParams);

	this->CreateDevice();
	this->CreateResources();

	this->window->OnResize = std::bind(&D3DEngine::WindowResized, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
	this->window->OnFocusChange = std::bind(&D3DEngine::WindowFocusChanged, this, std::placeholders::_1, std::placeholders::_2);
	this->window->OnClosing = std::bind(&D3DEngine::WindowClosing, this, std::placeholders::_1);
	this->window->OnPaint = std::bind(&D3DEngine::WindowPaint, this, std::placeholders::_1);
}
