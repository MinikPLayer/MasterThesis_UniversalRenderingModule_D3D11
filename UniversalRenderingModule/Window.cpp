#include "pch.h"
#include "Window.h"
#include <spdlog/spdlog.h>
#include "Log.h"

LRESULT CALLBACK WndProdDispatcher(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    Window* window = nullptr;

    if (message == WM_NCCREATE)
    {
        window = (Window*)(((LPCREATESTRUCT)lParam)->lpCreateParams);

        SetLastError(0);
        if (!SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)window) && GetLastError() != 0) {
			auto lastError = GetLastError();
			Logger::GetFatalLogger()->critical("Failed to set window user data: {}", lastError);
        }
    }
    else {
        window = (Window*)(GetWindowLongPtr(hwnd, GWLP_USERDATA));
    }

    if (window != nullptr) {
        return window->WndProc(hwnd, message, wParam, lParam);
    }
    else {
        return DefWindowProc(hwnd, message, wParam, lParam);
    }
}

bool Window::Create(WindowCreationParams p) {
    WNDCLASSEXW wcex = {};
    wcex.cbSize = sizeof(WNDCLASSEXW);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProdDispatcher;
    wcex.hIcon = p.icon;
    wcex.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    wcex.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
    wcex.lpszClassName = L"UniversalRenderingModuleWindowClass";
    wcex.hIconSm = p.icon;

    if (!RegisterClassExW(&wcex))
        return false;

    this->width = p.width;
    this->height = p.height;
	RECT rc = { 0, 0, (LONG)p.width, (LONG)p.height };
	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);

    std::wstring wTitle = std::wstring(p.title.begin(), p.title.end());
    this->handle = CreateWindowExW(0, L"UniversalRenderingModuleWindowClass", wTitle.c_str(), WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top,
        nullptr, nullptr, p.hInstance,
        this);

	if (!this->handle)
		return false;
}

LRESULT Window::WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message)
    {
    case WM_PAINT:
        if (this->OnPaint)
            this->OnPaint(*this);

        break;

    case WM_SIZE:
    {
        auto oldSize = Size2i(this->width, this->height);
        if (wParam == SIZE_MINIMIZED)
        {
            if (!this->width && !this->height)
            {
                this->width = LOWORD(lParam);
                this->height = HIWORD(lParam);
            }
        }
        else
        {
            this->width = LOWORD(lParam);
            this->height = HIWORD(lParam);
        }

        if (this->OnResize)
            this->OnResize(*this, oldSize, Size2i(this->width, this->height));

        break;
    }

    case WM_ACTIVATEAPP:
    {
        auto isFocused = wParam;
		if(this->OnFocusChange)
			this->OnFocusChange(*this, isFocused);

        break;
    }

    case WM_SYSKEYDOWN:
    {
        // TODO: Add OnKeyPressed().


		// Toggle fullscreen on ALT+ENTER
        //     if (wParam == VK_RETURN && (lParam & 0x60000000) == 0x20000000) {
		//          this->SetFullscreen(!this->IsFullscreen());
        //     }

        break;
    }

	// TODO: Allow for multiple windows to be created, so WM_DESTROY shouldn't close the app when other windows are opened.
    case WM_DESTROY:
        if (this->OnClosing)
        {
			auto preventClosing = this->OnClosing(*this);
            if (preventClosing)
                break;
        }

        PostQuitMessage(0);
        break;

    }

	return DefWindowProc(hwnd, message, wParam, lParam);
}

void Window::Show() {
	ShowWindow(this->handle, SW_SHOWNORMAL);
	UpdateWindow(this->handle);
}

void Window::Hide() {
	ShowWindow(this->handle, SW_HIDE);
	UpdateWindow(this->handle);
}

void Window::SetFullscreen(bool fullscreen) {
    if (fullscreen) {
        SetWindowLongPtr(this->handle, GWL_STYLE, WS_POPUP);
		//SetWindowLongPtr(this->handle, GWL_EXSTYLE, WS_EX_TOPMOST);

		SetWindowPos(this->handle, HWND_TOP, 0, 0, 0, 0, WS_EX_TOPMOST | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
		ShowWindow(this->handle, SW_SHOWMAXIMIZED);
	}
    else {
        SetWindowLongPtr(this->handle, GWL_STYLE, WS_OVERLAPPEDWINDOW);
        SetWindowLongPtr(this->handle, GWL_EXSTYLE, 0);

        ShowWindow(this->handle, SW_SHOWNORMAL);
        SetWindowPos(this->handle, HWND_TOP, 0, 0, this->width, this->height, SWP_NOMOVE | SWP_NOZORDER | SWP_FRAMECHANGED);
    }
}

bool Window::IsFullscreen() {
    auto style = GetWindowLongPtr(this->handle, GWL_STYLE);
	auto exStyle = GetWindowLongPtr(this->handle, GWL_EXSTYLE);

	return ((style & WS_POPUP) == WS_POPUP) && ((exStyle & WS_EX_TOPMOST) == WS_EX_TOPMOST);
}

int Window::RunHandlerLoop() {
    MSG msg = {};

    while (msg.message != WM_QUIT) {
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
    }

    return msg.wParam;
}

Window::Window(WindowCreationParams p, bool show) {
    if (!this->Create(p)) {
		spdlog::critical("Failed to create a window.");
		throw std::runtime_error("Failed to create a window");
    }

	if (show)
		this->Show();
}