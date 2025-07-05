#pragma once

#include "ITest.h"
#include <URM/Engine/LightObject.h>
#include <URM/Engine/Engine.h>
#include <URM/Core/Window.h>

class LightsTest : public AutoTest {
	int currentLightCount = 0;
	std::shared_ptr<URM::Engine::SceneObject> lightsObject;

protected:
	unsigned int GetCount() override {
		return currentLightCount;
	}

	void AddLight() {
		auto newLight = lightsObject->AddChild(new URM::Engine::LightObject());
		newLight->color = Color(
			sin(currentLightCount * 0.9f) * 0.5f + 0.5f,
			sin(currentLightCount * 0.65f) * 0.5f + 0.5f,
			sin(currentLightCount * 0.5f) * 0.5f + 0.5f
		);
		newLight->ambientIntensity = 0.0f;
		newLight->diffuseIntensity = 0.0f;
		newLight->specularIntensity = 0.5f;
		currentLightCount++;

		const auto distance = 10.0f;
		for (int i = 0; i < currentLightCount; i++) {
			auto light = lightsObject->GetChildByIndex(i);
			auto x = sin(i * 0.5f) * distance * i / currentLightCount;
			auto z = cos(i * 0.5f) * distance * i / currentLightCount;
			light->GetTransform().SetLocalPosition({ x, -0.9f, z });
		}
	}

	bool IncreaseCount() override {
		AddLight();

		return true;
	}

	bool DecreaseCount() override {
		if(currentLightCount == 0) {
			return false;
		}

		auto light = lightsObject->GetChildByIndex(currentLightCount - 1);
		light->Destroy();
		currentLightCount--;
		return true;
	}

	void OnInit(URM::Engine::Engine& engine) override {
		auto root = engine.GetScene().GetRoot().lock();
		lightsObject = root->AddChild(new URM::Engine::SceneObject());

		root->AddChild(new URM::Engine::ModelObject("suzanne.glb"));
		auto cube = root->AddChild(new URM::Engine::ModelObject("cube.glb"));
		cube->GetTransform().SetLocalScale({ 10.0f, 1.0f, 10.0f });
		cube->GetTransform().SetLocalPosition({ 0.0f, -2.0f, 0.0f });

		auto camera = engine.GetScene().GetMainCamera().lock();
		camera->GetTransform().SetPosition({ 0, 2, 5 });
		camera->GetTransform().LookAt({ 0, 0, 0 });

		const unsigned int startLightsCount = 16;
		for (int i = 0; i < startLightsCount; i++) {
			AddLight();
		}
	}

	void OnUpdate(URM::Engine::Engine& engine) override {
		lightsObject->GetTransform().SetRotation({
			0.0f,
			engine.GetTimer().GetElapsedTime() * 45.0f,
			0.0f
		});
	}

public:
	virtual URM::Core::WindowCreationParams GetWindowParams(HINSTANCE instance) const {
		return URM::Core::WindowCreationParams(1920, 1080, "URM Benchmarks", instance);
	}
};