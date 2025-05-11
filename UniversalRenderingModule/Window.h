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

	int width = -1;
	int height = -1;

	std::function<void(Window&)> OnPaint = {};
	std::function<void(Window& window, Size2i oldSize, Size2i newSize)> OnResize = {};

	std::function<void(Window& window, bool isFocused)> OnFocusChange = {};

	// Return true to prevent the window from closing.
	std::function<bool(Window& window)> OnClosing = {};

	bool Create(WindowCreationParams params);

	LRESULT WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

	HWND GetHandle() {
		return this->handle;
	}

	int RunHandlerLoop();
public:

	void Show();
	void Hide();

	void SetFullscreen(bool fullscreen);
	bool IsFullscreen();

	Size2i GetSize() {
		return Size2i(this->width, this->height);
	}

	Window(WindowCreationParams params, bool show = true);
};

