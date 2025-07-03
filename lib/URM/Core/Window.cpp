#include "pch.h"
#include "Window.h"
#include <spdlog/spdlog.h>
#include "Log.h"

namespace URM::Core {
	LRESULT CALLBACK WndProdDispatcher(const HWND hwnd, const UINT message, const WPARAM wParam, const LPARAM lParam) {
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

	static std::wstring FindUnusedClassName(const HINSTANCE hInstance, std::string base) {
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
		const auto className = FindUnusedClassName(p.hInstance, p.title);
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

		this->mWidth = p.width;
		this->mHeight = p.height;
		RECT rc = {0, 0, static_cast<LONG>(p.width), static_cast<LONG>(p.height)};
		AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);

		auto wTitle = std::wstring(p.title.begin(), p.title.end());
		this->mHandle = CreateWindowExW(0,
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

		if (!this->mHandle)
			return false;

		return true;
	}

	LRESULT Window::WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
		switch (message) {
			case WM_LBUTTONDOWN:
				spdlog::info("Left mouse button down at ({}, {})", LOWORD(lParam), HIWORD(lParam));
				break;

			case WM_PAINT:
				if (mIsResizing) {
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
				this->mOldSize = Size2i(this->mWidth, this->mHeight);
				if (wParam == SIZE_MINIMIZED) {
					if (!this->mWidth && !this->mHeight) {
						this->mWidth = LOWORD(lParam);
						this->mHeight = HIWORD(lParam);
					}
				}
				else {
					this->mWidth = LOWORD(lParam);
					this->mHeight = HIWORD(lParam);
				}

				if (this->OnResize)
					this->OnResize(*this, this->mOldSize, Size2i(this->mWidth, this->mHeight));

				break;
			}

			case WM_ENTERSIZEMOVE:
				this->mIsResizing = true;
				this->mOldSize = Size2i(this->mWidth, this->mHeight);
				break;

			case WM_EXITSIZEMOVE:
				this->mIsResizing = false;
				if (this->OnResize)
					this->OnResize(*this, this->mOldSize, Size2i(this->mWidth, this->mHeight));
				break;

			case WM_ACTIVATE:
			case WM_ACTIVATEAPP: {
				DirectX::Keyboard::ProcessMessage(message, wParam, lParam);
				auto isFocused = wParam;
				if (this->OnFocusChange)
					this->OnFocusChange(*this, isFocused);

				break;
			}

			case WM_SYSKEYDOWN:
			case WM_KEYDOWN:
			case WM_KEYUP:
			case WM_SYSKEYUP:
				DirectX::Keyboard::ProcessMessage(message, wParam, lParam);
				break;

			case WM_CLOSE:
				if (this->OnCloseRequested) {
					if (this->OnCloseRequested(*this))
						return 0;
				}

				DestroyWindow(hwnd);
				break;

			case WM_DESTROY:
				this->mIsDestroyed = true;
				break;
			
			default: break;

		}

		return DefWindowProc(hwnd, message, wParam, lParam);
	}

	void Window::Show() const {
		ShowWindow(this->mHandle, SW_SHOWNORMAL);
		UpdateWindow(this->mHandle);
	}

	void Window::Hide() const {
		ShowWindow(this->mHandle, SW_HIDE);
		UpdateWindow(this->mHandle);
	}

	void Window::Close(bool ignoreOnCloseRequested) {
		if (!ignoreOnCloseRequested && this->OnCloseRequested) {
			if (this->OnCloseRequested(*this))
				return;
		}

		if (this->mHandle) {
			DestroyWindow(this->mHandle);
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
		if (this->mHandle) {
			DestroyWindow(this->mHandle);
			this->mHandle = nullptr;
		}
	}
}
