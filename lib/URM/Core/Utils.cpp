#include "pch.h"
#include "Utils.h"

const URM::Core::Size2i URM::Core::Size2i::ZERO = Size2i(0, 0);

bool URM::Core::Size2i::operator==(const Size2i& s2) const {
	return this->height == s2.height && this->width == s2.width;
}

bool URM::Core::Size2i::operator!=(const Size2i& s2) const {
	return !(*this == s2);
}
