#pragma once
#include <Windows.h>
#include "Utils.h"
#include <functional>

struct WindowCreationParams {
	int width;
	int height;
	std::string title;

	HINSTANCE hInstance;
	HICON icon;

	WindowCreationParams(int width, int height, std::string title, HINSTANCE hInstance, HICON icon = NULL):	
		width(width), 
		height(height), 
		title(title), 
		hInstance(hInstance),
		icon(icon) {}
};

class Window {
	friend class D3DEngine;
	friend LRESULT WndProdDispatcher(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

	HWND handle = NULL;

	Size2i oldSize = { -1, -1 };
	bool isResizing = false;

	int width = -1;
	int height = -1;

	bool isDestroyed = false;

	std::function<void(Window&)> OnPaint = {};
	std::function<void(Window& window, Size2i oldSize, Size2i newSize)> OnResize = {};

	std::function<void(Window& window, bool isFocused)> OnFocusChange = {};

	// Return true to prevent the window from closing.
	std::function<bool(Window& window)> OnCloseRequested = {};

	bool Create(WindowCreationParams params);

	LRESULT WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

public:
	void PollEvents();

	HWND GetHandle() {
		return this->handle;
	}

	bool IsDestroyed() {
		return isDestroyed;
	}

	void Show();
	void Hide();
	void Close(bool ignoreOnCloseRequested = false);

	Size2i GetSize() {
		return Size2i(this->width, this->height);
	}

	Window(WindowCreationParams params, bool show = true);
};

