#pragma once
#include <Windows.h>
#include "Utils.h"
#include <functional>
#include "pch.h"

namespace URM::Core {
	struct WindowCreationParams {
		int width;
		int height;
		std::string title;

		HINSTANCE hInstance;
		HICON icon;

		WindowCreationParams(int width, int height, const std::string& title, HINSTANCE hInstance, HICON icon = nullptr) : width(width),
		                                                                                                            height(height),
		                                                                                                            title(title),
		                                                                                                            hInstance(hInstance),
		                                                                                                            icon(icon) {}
	};

	class Window : NonCopyable {
		friend class D3DCore;
		friend LRESULT WndProdDispatcher(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

		HWND mHandle = nullptr;

		Size2i mOldSize = {-1, -1};
		bool mIsResizing = false;

		int mWidth = -1;
		int mHeight = -1;

		bool mIsDestroyed = false;

		std::function<void(Window&)> OnPaint = {};
		std::function<void(Window& window, Size2i oldSize, Size2i newSize)> OnResize = {};

		std::function<void(Window& window, bool isFocused)> OnFocusChange = {};

		// Return true to prevent the window from closing.
		std::function<bool(Window& window)> OnCloseRequested = {};

		bool Create(WindowCreationParams params);

		LRESULT WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
	public:
		static void PollEvents();

		HWND GetHandle() const {
			return this->mHandle;
		}

		bool IsDestroyed() const {
			return mIsDestroyed;
		}

		void Show() const;
		void Hide() const;
		void Close(bool ignoreOnCloseRequested = false);

		Size2i GetSize() const {
			return Size2i(this->mWidth, this->mHeight);
		}

		Window(const WindowCreationParams& params, bool show = true);
		~Window() override;
	};
}
