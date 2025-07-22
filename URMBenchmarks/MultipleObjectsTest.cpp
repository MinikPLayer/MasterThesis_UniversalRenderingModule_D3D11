#include "MultipleObjectsTest.h"
#include <URM/Engine/ModelObject.h>

void MultipleObjectsTest::AddObject() {
    auto newObject = mModelsParent->AddChild(new URM::Engine::ModelObject("cube.glb"));
	newObject->GetTransform().SetLocalScale({ 0.01f, 0.01f, 0.01f });
    mCurrentObjectCount++;
}

void MultipleObjectsTest::OnInit(URM::Engine::Engine& engine) {
	this->mModelsParent = engine.GetScene().GetRoot().lock()->AddChild(new URM::Engine::SceneObject());

	this->IncreaseCount(1);

	auto light = new URM::Engine::LightObject();
	light->attenuationExponent = 0.0f;
	light->GetTransform().SetPosition({ 0, 2, 0});
	engine.GetScene().GetRoot().lock()->AddChild(light);

	auto camera = engine.GetScene().GetMainCamera().lock();
	camera->GetTransform().SetPosition({ 0.01f, 20.0f, 0.01f });
	camera->GetTransform().LookAt({ 0, 0, 0 });
}

void MultipleObjectsTest::OnUpdate(URM::Engine::Engine& engine) {
	//this->mModelsParent->GetTransform().SetLocalRotation({
	//	0.0f,
	//	engine.GetTimer().GetElapsedTime() * 90.0f,
	//	0.0f
	//});
}

bool MultipleObjectsTest::IncreaseCount(size_t amount) {
	for(size_t i = 0; i < amount; i++) {
		this->AddObject();
	}

	const auto distance = 10.0f;
	for (int i = 0; i < this->mCurrentObjectCount; i++) {
		auto model = this->mModelsParent->GetChildByIndex(i);
		auto x = sin(i * 0.5f) * distance * i / this->mCurrentObjectCount;
		auto z = cos(i * 0.5f) * distance * i / this->mCurrentObjectCount;
		model->GetTransform().SetLocalPosition({ x, -0.9f, z });
	}

	return true;
}

bool MultipleObjectsTest::DecreaseCount() {
	if (this->mCurrentObjectCount == 0) {
		return false;
	}

	auto model = this->mModelsParent->GetChildByIndex(this->mCurrentObjectCount - 1);
	model->Destroy();
	this->mCurrentObjectCount--;
	return true;
}

unsigned int MultipleObjectsTest::GetCount() {
    return this->mCurrentObjectCount;
}

// This test is likely to overshoot the target FPS, so we use a lower multiplier.
float MultipleObjectsTest::GetTargetCurveMultiplier() {
	return 0.25f;
}
