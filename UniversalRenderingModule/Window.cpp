#include "pch.h"
#include "Window.h"
#include <spdlog/spdlog.h>
#include "Log.h"

LRESULT CALLBACK WndProdDispatcher(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	Window* window = reinterpret_cast<Window*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
    if (window == nullptr) {
		spdlog::critical("[Internal error] Window pointer is not set as a Window USERDATA LongPtr.");
    }

	return window->WndProc(hwnd, message, wParam, lParam);
}

bool Window::Create(WindowCreationParameters p) {
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
    return LRESULT();
}

void Window::Show() {
	ShowWindow(this->handle, SW_SHOWNORMAL);
	UpdateWindow(this->handle);
}

void Window::Hide() {
	ShowWindow(this->handle, SW_HIDE);
	UpdateWindow(this->handle);
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

Window::Window(WindowCreationParameters p, bool show) {
	this->Create(p);

	if (show)
		this->Show();
}