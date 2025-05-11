#pragma once
#include <Windows.h>
#include "Utils.h"

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
	HWND handle = NULL;

	int width = -1;
	int height = -1;

	bool Create(WindowCreationParams params);

	LRESULT WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

	friend LRESULT WndProdDispatcher(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

public:
	HWND GetHandle() {
		return this->handle;
	}

	void Show();
	void Hide();

	void SetFullscreen(bool fullscreen);
	bool IsFullscreen();

	Size2i GetSize() {
		return Size2i(this->width, this->height);
	}

	int RunHandlerLoop();

	Window(WindowCreationParams params, bool show = true);
};

