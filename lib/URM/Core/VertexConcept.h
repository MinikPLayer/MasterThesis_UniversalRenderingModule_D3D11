#pragma once
#include <concepts>
#include <vector>
#include <d3d11.h>

namespace URM::Core {
	template<typename T>
	concept VertexTypeConcept = requires {
		{ T::GetInputLayout() } -> std::same_as<std::vector<D3D11_INPUT_ELEMENT_DESC>>;
	};
}
