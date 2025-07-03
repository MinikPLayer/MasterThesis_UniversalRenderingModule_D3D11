#pragma once

#include "ITest.h"
#include <URM/Engine/LightObject.h>
#include <URM/Engine/Engine.h>

class LightsTest : public ITest {
	std::shared_ptr<URM::Engine::SceneObject> lightsObject;

public:
	void Init(URM::Engine::Engine& engine) override {
		auto root = engine.GetScene().GetRoot().lock();
		lightsObject = root->AddChild(new URM::Engine::SceneObject());

		root->AddChild(new URM::Engine::ModelObject("suzanne.glb"));
		auto cube = root->AddChild(new URM::Engine::ModelObject("cube.glb"));
		cube->GetTransform().SetLocalScale({ 10.0f, 1.0f, 10.0f });
		cube->GetTransform().SetLocalPosition({ 0.0f, -2.0f, 0.0f });

		auto camera = engine.GetScene().GetMainCamera().lock();
		camera->GetTransform().SetPosition({ 0, 2, 5 });
		camera->GetTransform().LookAt({ 0, 0, 0 });

		const unsigned int lightsCount = 1024;
		for (int i = 0; i < lightsCount; i++) {
			auto light = lightsObject->AddChild(new URM::Engine::LightObject());
			const auto distance = 10.0f;
			auto x = sin(i * 0.5f) * distance * i / lightsCount;
			auto z = cos(i * 0.5f) * distance * i / lightsCount;
			light->GetTransform().SetPosition({ x, -0.9f, z});
			light->color = Color(
				sin(i * 0.9f) * 0.5f + 0.5f, 
				sin(i * 0.65f) * 0.5f + 0.5f, 
				sin(i * 0.5f) * 0.5f + 0.5f
			);
			light->ambientIntensity = 0.0f;
			light->diffuseIntensity = 0.0f;
			light->specularIntensity = 0.5f;
		}
	}

	void Update(URM::Engine::Engine& engine) override {
		lightsObject->GetTransform().SetRotation({
			0.0f,
			engine.GetTimer().GetElapsedTime() * 45.0f,
			0.0f
		});
	}
};