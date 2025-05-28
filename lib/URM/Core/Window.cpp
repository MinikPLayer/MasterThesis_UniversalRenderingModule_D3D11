#include "pch.h"
#include "Window.h"
#include <spdlog/spdlog.h>
#include "Log.h"

namespace URM::Core {
	LRESULT CALLBACK WndProdDispatcher(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
		Window* window;
		if (message == WM_NCCREATE) {
			window = static_cast<Window*>(reinterpret_cast<LPCREATESTRUCT>(lParam)->lpCreateParams);

			SetLastError(0);
			if (!SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(window)) && GetLastError() != 0) {
				auto lastError = GetLastError();
				Logger::GetFatalLogger()->critical("Failed to set window user data: {}", lastError);
			}
		}
		else {
			window = reinterpret_cast<Window*>((GetWindowLongPtr(hwnd, GWLP_USERDATA)));
		}

		if (window != nullptr) {
			return window->WndProc(hwnd, message, wParam, lParam);
		}
		return DefWindowProc(hwnd, message, wParam, lParam);
	}

	static std::wstring FindUnusedClassName(HINSTANCE hInstance, std::string base) {
		auto className = std::wstring(base.begin(), base.end());

		constexpr int triesCount = 10000;
		for (int i = 0; i < triesCount; i++) {
			WNDCLASS wndClass = {};
			auto isRegistered = GetClassInfo(hInstance, className.c_str(), &wndClass);
			if (!isRegistered) {
				return className;
			}

			className = std::wstring(base.begin(), base.end()) + L"_" + std::to_wstring(i);
		}

		Logger::GetFatalLogger()->critical("Failed to find an unused class name after {} tries.", triesCount);
		throw std::runtime_error("Failed to find an unused class name");
	}

	bool Window::Create(WindowCreationParams p) {
		auto className = FindUnusedClassName(p.hInstance, p.title);
		spdlog::trace("Using class name \"{}\"", StringUtils::WStringToString(className));

		WNDCLASSEXW wcex = {};
		wcex.cbSize = sizeof(WNDCLASSEXW);
		wcex.style = CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc = WndProdDispatcher;
		wcex.hIcon = p.icon;
		wcex.hInstance = p.hInstance;
		wcex.hCursor = LoadCursorW(nullptr, IDC_ARROW);
		wcex.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
		wcex.lpszClassName = className.c_str();
		wcex.hIconSm = p.icon;

		if (!RegisterClassExW(&wcex))
			return false;

		this->width = p.width;
		this->height = p.height;
		RECT rc = {0, 0, static_cast<LONG>(p.width), static_cast<LONG>(p.height)};
		AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);

		auto wTitle = std::wstring(p.title.begin(), p.title.end());
		this->handle = CreateWindowExW(0,
		                               className.c_str(),
		                               wTitle.c_str(),
		                               WS_OVERLAPPEDWINDOW,
		                               CW_USEDEFAULT,
		                               CW_USEDEFAULT,
		                               rc.right - rc.left,
		                               rc.bottom - rc.top,
		                               nullptr,
		                               nullptr,
		                               p.hInstance,
		                               this);

		if (!this->handle)
			return false;

		return true;
	}

	LRESULT Window::WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
		switch (message) {
			case WM_PAINT:
				if (isResizing) {
					if (this->OnPaint)
						this->OnPaint(*this);
				}
				else {
					PAINTSTRUCT ps;
					std::ignore = BeginPaint(hwnd, &ps);
					EndPaint(hwnd, &ps);
				}

				break;

			case WM_SIZE: {
				this->oldSize = Size2i(this->width, this->height);
				if (wParam == SIZE_MINIMIZED) {
					if (!this->width && !this->height) {
						this->width = LOWORD(lParam);
						this->height = HIWORD(lParam);
					}
				}
				else {
					this->width = LOWORD(lParam);
					this->height = HIWORD(lParam);
				}

				if (this->OnResize)
					this->OnResize(*this, this->oldSize, Size2i(this->width, this->height));

				break;
			}

			case WM_ENTERSIZEMOVE:
				this->isResizing = true;
				this->oldSize = Size2i(this->width, this->height);
				break;

			case WM_EXITSIZEMOVE:
				this->isResizing = false;
				if (this->OnResize)
					this->OnResize(*this, this->oldSize, Size2i(this->width, this->height));
				break;

			case WM_ACTIVATEAPP: {
				auto isFocused = wParam;
				if (this->OnFocusChange)
					this->OnFocusChange(*this, isFocused);

				break;
			}

			case WM_SYSKEYDOWN: {
				// TODO: Add OnKeyPressed().
				// TODO: Add fullscreen toggle

				// Toggle fullscreen on ALT+ENTER
				//if (wParam == VK_RETURN && (lParam & 0x60000000) == 0x20000000) {
				//    this->SetFullscreen(!this->IsFullscreen());
				//}

				break;
			}

			case WM_CLOSE:
				if (this->OnCloseRequested) {
					if (this->OnCloseRequested(*this))
						return 0;
				}

				DestroyWindow(hwnd);
				break;

			case WM_DESTROY:
				this->isDestroyed = true;
				break;
			
			default: break;

		}

		return DefWindowProc(hwnd, message, wParam, lParam);
	}

	void Window::Show() const {
		ShowWindow(this->handle, SW_SHOWNORMAL);
		UpdateWindow(this->handle);
	}

	void Window::Hide() const {
		ShowWindow(this->handle, SW_HIDE);
		UpdateWindow(this->handle);
	}

	void Window::Close(bool ignoreOnCloseRequested) {
		if (!ignoreOnCloseRequested && this->OnCloseRequested) {
			if (this->OnCloseRequested(*this))
				return;
		}

		if (this->handle) {
			DestroyWindow(this->handle);
		}
	}

	void Window::PollEvents() {
		MSG msg = {};

		while (msg.message != WM_QUIT) {
			if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			else {
				break;
			}
		}
	}

	Window::Window(const WindowCreationParams& p, bool show) {
		if (!this->Create(p)) {
			spdlog::critical("Failed to create a window.");
			throw std::runtime_error("Failed to create a window");
		}

		if (show)
			this->Show();
	}

	Window::~Window() {
		if (this->handle) {
			DestroyWindow(this->handle);
			this->handle = nullptr;
		}
	}
}
