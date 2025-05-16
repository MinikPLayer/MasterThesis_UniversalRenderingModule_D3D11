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

struct Pos2f {
	float x, y;

	Pos2f(float x, float y) : x(x), y(y) {}

	const static Pos2f ZERO;
};

class StringUtils {
	StringUtils() = delete;

public:
	// Converts UTF-8 wstring to string.
	static std::string WStringToString(std::wstring data) {
		std::vector<char> Bufer(data.size());
		std::use_facet<std::ctype<wchar_t>>(std::locale()).narrow(data.data(), data.data() + data.size(), '?', Bufer.data());
		return std::string(Bufer.data(), Bufer.size());
	}

	static std::wstring StringToWString(std::string data) {
		std::vector<wchar_t> buf(data.size());
		std::use_facet<std::ctype<wchar_t>>(std::locale()).widen(data.data(), data.data() + data.size(), buf.data());
		return std::wstring(buf.data(), buf.size());
	}
};

