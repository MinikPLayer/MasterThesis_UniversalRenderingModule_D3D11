#pragma once
#include <math.h>
#include <string>
#include <vector>
#include <locale>

class NonCopyable {
public:
	NonCopyable(const NonCopyable&) = delete;
	NonCopyable& operator=(NonCopyable const&) = delete;
	NonCopyable() {}
};

namespace URM::Core {
	struct Size2i {
		int width;
		int height;

		Size2i(int width, int height) : width(width), height(height) {}
		Size2i() {
			this->width = 0;
			this->height = 0;
		}

		bool operator==(Size2i& s2);
		bool operator!=(Size2i& s2);

		const static Size2i ZERO;
	};

	class TypeUtils {
		TypeUtils() = delete;
	public:
		template <typename T>
		static inline size_t GetTypeCode() {
			return typeid(T).hash_code();
		}

		template <typename T>
		static inline std::string GetTypeName() {
			return typeid(T).name();
		}

		template <typename T>
		static inline bool IsType(const std::size_t typeCode) {
			return TypeUtils::GetTypeCode<T>() == typeCode;
		}
	};

	class FloatUtils {
	public:
		template <typename T>
		static int Sign(T val) {
			return (T(0) < val) - (val < T(0));
		}

		bool static inline IsEqualApproximate(float a, float b, float epsilon = 0.00001f) {
			return fabs(a - b) <= ((fabs(a) > fabs(b) ? fabs(b) : fabs(a)) * epsilon);
		}
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

		static std::string GetDirectoryFromPath(std::string path) {
			auto lastSlash = path.find_last_of("/\\");
			if (lastSlash == std::string::npos) {
				return "";
			}
			return path.substr(0, lastSlash);
		}
	};
}