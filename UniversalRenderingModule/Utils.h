#pragma once

struct Size2i {
	int width;
	int height;

	Size2i(int width, int height) : width(width), height(height) {}

	const static Size2i ZERO;
};

struct Pos2i {
	int x, y;

	Pos2i(int x, int y) : x(x), y(y) {}

	const static Pos2i ZERO;
};