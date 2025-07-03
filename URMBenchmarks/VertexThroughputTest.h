#pragma once

#include "ITest.h"

#include <URM/Engine/ModelObject.h>
#include <URM/Engine/MeshObject.h>

// Model source: https://www.fab.com/listings/36463603-685c-4605-88c5-ad7b17355143
class VertexThroughputTest : public ITest {
	std::shared_ptr<URM::Engine::SceneObject> mModel;

	UINT CalculateVerticesCount(std::shared_ptr<URM::Engine::ModelObject> model) const {
		auto children = model->GetChildrenByType<URM::Engine::MeshObject>(true);
		UINT count = 0;
		for (const auto& child : children) {
			count += child->GetMesh().GetVerticesCount();
		}

		return count;
	}

public:
	URM::Core::WindowCreationParams GetWindowParams(HINSTANCE instance) const override {
		return URM::Core::WindowCreationParams(640, 480, "URM Vertex Throughput Test", instance);
	}

	void Init(URM::Engine::Engine& engine) override {
		auto root = engine.GetScene().GetRoot().lock();

		const float scale = 8.0f;
		const Vector3 modelSize = { 0, scale, 0 };
		auto light = new URM::Engine::LightObject();
		light->GetTransform().SetPosition({ 0, 3, 5 });
		root->AddChild(light);
		mModel = root->AddChild(new URM::Engine::SceneObject());
		auto model = mModel->AddChild(new URM::Engine::ModelObject("roman_marble_3.000.000_triangles.glb"));
		model->GetTransform().SetLocalScale({ scale, scale, scale });
		//model->GetTransform().SetLocalPosition({ -modelSize.x / 2, -modelSize.y / 2, -modelSize.z / 2 });
		mModel->GetTransform().SetPosition({ 2, -2, 0 });

		auto verticesCount = CalculateVerticesCount(model);
		spdlog::info("[VertexThroughputTest] Model loaded with {} vertices", verticesCount);

		auto camera = engine.GetScene().GetMainCamera().lock();
		camera->GetTransform().SetPosition({ 8, 4, 8 });
		camera->GetTransform().LookAt({ 0, 0, 0 });
	}

	void Update(URM::Engine::Engine& engine) override {
		mModel->GetTransform().SetLocalRotation({
			0.0f,
			engine.GetTimer().GetElapsedTime() * 90.0f,
			0.0f
		});
	}
};