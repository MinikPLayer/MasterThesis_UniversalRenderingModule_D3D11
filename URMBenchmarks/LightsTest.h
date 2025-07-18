#pragma once

#include "ITest.h"
#include <URM/Engine/LightObject.h>
#include <URM/Engine/Engine.h>
#include <URM/Core/Window.h>

class LightsTest : public AutoTest {
	int currentLightCount = 0;
	std::shared_ptr<URM::Engine::SceneObject> lightsObject;

	bool mEnableAutoScaling = true;
	bool mEnableRotation = true;
protected:
	unsigned int GetCount() override {
		return currentLightCount;
	}

	void AddLight() {
		auto newLight = lightsObject->AddChild(new URM::Engine::LightObject());
		if (this->mEnableAutoScaling) {
			newLight->color = Color(
				sin(currentLightCount * 0.9f) * 0.5f + 0.5f,
				sin(currentLightCount * 0.65f) * 0.5f + 0.5f,
				sin(currentLightCount * 0.5f) * 0.5f + 0.5f
			);
		}
		else {
			newLight->color = this->currentLightCount < 64 ? Color(1.0f, 0.0f, 0.0f) : Color(0.0f, 1.0f, 0.0f);
		}

		newLight->ambientIntensity = 0.0f;
		newLight->diffuseIntensity = 0.0f;
		newLight->specularIntensity = 0.5f;
		currentLightCount++;
	}

	bool AddLights(size_t amount) {
		for (size_t i = 0; i < amount; i++) {
			AddLight();
		}

		auto distance = this->mEnableAutoScaling ? 10.0f : 4.0f;
		for (int i = 0; i < currentLightCount; i++) {
			auto light = lightsObject->GetChildByIndex(i);
			auto x = sin(i * 0.5f) * distance * i / currentLightCount;
			auto z = cos(i * 0.5f) * distance * i / currentLightCount;
			light->GetTransform().SetLocalPosition({ x, -0.9f, z });
		}

		return true;
	}

	bool IncreaseCount(size_t amount) override {
		if (!this->mEnableAutoScaling) {
			return false;
		}

		AddLights(amount);
	}

	bool DecreaseCount() override {
		if (!this->mEnableAutoScaling) {
			return false;
		}

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

		auto suzanne = root->AddChild(new URM::Engine::ModelObject("suzanne.glb"));
		suzanne->GetChildrenByType<URM::Engine::MeshObject>(true)[0]->material = std::shared_ptr<URM::Core::MaterialSimple>(new URM::Core::MaterialSimple(engine.GetCore(), URM::Core::MaterialSimpleData(1024)));

		auto cube = root->AddChild(new URM::Engine::ModelObject("cube.glb"));
		cube->GetChildrenByType<URM::Engine::MeshObject>(true)[0]->material = std::shared_ptr<URM::Core::MaterialSimple>(new URM::Core::MaterialSimple(engine.GetCore(), URM::Core::MaterialSimpleData(128)));

		cube->GetTransform().SetLocalScale({ 10.0f, 1.0f, 10.0f });
		cube->GetTransform().SetLocalPosition({ 0.0f, -2.0f, 0.0f });

		auto camera = engine.GetScene().GetMainCamera().lock();
		camera->GetTransform().SetPosition(this->mEnableAutoScaling ? Vector3(0, 2, 5) : Vector3(0, 1, 5));
		camera->GetTransform().LookAt({ 0, 0, 0 });

		const unsigned int startLightsCount = this->mEnableAutoScaling ? 16 : 128;
		AddLights(startLightsCount);
	}

	void OnUpdate(URM::Engine::Engine& engine) override {
		if (!this->mEnableRotation) {
			return;
		}

		lightsObject->GetTransform().SetRotation({
			0.0f,
			engine.GetTimer().GetElapsedTime() * 45.0f,
			0.0f
		});
	}

	virtual float GetTargetCurveMultiplier() {
		return 0.7f;
	}

public:
	virtual URM::Core::WindowCreationParams GetWindowParams(HINSTANCE instance) const {
		return URM::Core::WindowCreationParams(1920, 1080, "URM Benchmarks", instance);
	}

	LightsTest(bool enableAutoScaling = true, bool enableRotation = true) {
		this->mEnableAutoScaling = enableAutoScaling;
		this->mEnableRotation = enableRotation;
	}
};