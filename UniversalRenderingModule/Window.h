#pragma once
#include <Windows.h>

struct WindowCreationParameters {
	int width;
	int height;
	std::string title;

	HINSTANCE hInstance;
	HICON icon;

	WindowCreationParameters(int width, int height, std::string title, HINSTANCE hInstance, HICON icon = NULL):	
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

	bool Create(WindowCreationParameters params);

	LRESULT WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

	friend LRESULT WndProdDispatcher(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

public:
	HWND getHandle() {
		return this->handle;
	}

	void Show();
	void Hide();

	int RunHandlerLoop();

	Window(WindowCreationParameters params, bool show = true);
};

