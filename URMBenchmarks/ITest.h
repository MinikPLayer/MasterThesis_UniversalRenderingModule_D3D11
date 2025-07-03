#pragma once
#include <URM/Engine/Engine.h>

class ITest {
public:
	virtual void Init(URM::Engine::Engine& engine) = 0;
	virtual void Update(URM::Engine::Engine& engine) = 0;

	virtual URM::Core::WindowCreationParams GetWindowParams(HINSTANCE instance) const {
		return URM::Core::WindowCreationParams(1280, 720, "URM Benchmarks", instance);
	}

	virtual ~ITest() = default;
};