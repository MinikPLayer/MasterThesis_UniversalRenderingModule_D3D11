#pragma once

#include "ITest.h"

#include <URM/Engine/ModelObject.h>
#include <URM/Engine/MeshObject.h>

// Model source: https://www.fab.com/listings/36463603-685c-4605-88c5-ad7b17355143
class VertexThroughputTest : public AutoTest {
	std::shared_ptr<URM::Engine::SceneObject> mModelsObject;
	int mCurrentVaseCount = 0;
	UINT mPerVaseVerticesCount = 0;

	unsigned int GetScore() override {
		return mCurrentVaseCount * mPerVaseVerticesCount;
	}

	unsigned int GetCount() override {
		return mCurrentVaseCount;
	}

	void AddModel() {
		auto newModel = mModelsObject->AddChild(new URM::Engine::ModelObject("roman_marble_3.000.000_triangles.glb"));
		mPerVaseVerticesCount = CalculateVerticesCount(newModel);
		mCurrentVaseCount++;
	}

	bool IncreaseCount(size_t amount) override {
		for(size_t i = 0; i < amount; i++) {
			AddModel();
		}

		const auto distance = 10.0f;
		for (int i = 0; i < mCurrentVaseCount; i++) {
			auto model = mModelsObject->GetChildByIndex(i);
			auto x = sin(i * 0.5f) * distance * i / mCurrentVaseCount;
			auto z = cos(i * 0.5f) * distance * i / mCurrentVaseCount;
			model->GetTransform().SetLocalPosition({ x, -0.9f, z });
		}

		return true;
	}

	bool DecreaseCount() override {
		if (mCurrentVaseCount == 0) {
			return false;
		}

		auto model = mModelsObject->GetChildByIndex(mCurrentVaseCount - 1);
		model->Destroy();
		mCurrentVaseCount--;
		return true;
	}

	UINT CalculateVerticesCount(std::shared_ptr<URM::Engine::ModelObject> model) const {
		auto children = model->GetChildrenByType<URM::Engine::MeshObject>(true);
		UINT count = 0;
		for (const auto& child : children) {
			count += child->GetMesh().GetVerticesCount();
		}

		return count;
	}

	void OnInit(URM::Engine::Engine& engine) override {
		auto root = engine.GetScene().GetRoot().lock();

		const float scale = 8.0f;
		const Vector3 modelSize = { 0, scale, 0 };
		auto light = new URM::Engine::LightObject();
		light->GetTransform().SetPosition({ 0, 3, 5 });
		root->AddChild(light);
		mModelsObject = root->AddChild(new URM::Engine::SceneObject());
		IncreaseCount(1);

		//model->GetTransform().SetLocalPosition({ -modelSize.x / 2, -modelSize.y / 2, -modelSize.z / 2 });
		mModelsObject->GetTransform().SetPosition({ 2, 0, 0 });

		auto camera = engine.GetScene().GetMainCamera().lock();
		camera->GetTransform().SetPosition({ 8, 4, 8 });
		camera->GetTransform().LookAt({ 0, 0, 0 });
	}

	void OnUpdate(URM::Engine::Engine& engine) override {
		mModelsObject->GetTransform().SetLocalRotation({
			0.0f,
			engine.GetTimer().GetElapsedTime() * 90.0f,
			0.0f
		});
	}

public:
	URM::Core::WindowCreationParams GetWindowParams(HINSTANCE instance) const override {
		return URM::Core::WindowCreationParams(640, 480, "URM Vertex Throughput Test", instance);
	}
};