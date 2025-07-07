#include "MaterialTest.h"
#include <URM/Engine/ModelObject.h>
#include <URM/Engine/MeshObject.h>

void MaterialTest::OnInit(URM::Engine::Engine& engine) {
	auto root = engine.GetScene().GetRoot().lock();

	const int SPHERE_COUNT = 10;
	const float SPHERE_SEPARATION = 2.5f;

	const int MIN_ROUGHNESS = 1;
	const int MAX_ROUGHNESS = 256;

	mLight = std::shared_ptr<URM::Engine::LightObject>(new URM::Engine::LightObject());
	mLight->GetTransform().SetPosition({ 0, 0, SPHERE_COUNT * SPHERE_SEPARATION });
	mLight->diffuseIntensity = 0.1f;
	mLight->ambientIntensity = 0.0f;
	root->AddChild(mLight);
	mModelsObject = root->AddChild(new URM::Engine::SceneObject());

	for (int i = 0; i < SPHERE_COUNT; i++) {
		auto newModel = mModelsObject->AddChild(new URM::Engine::ModelObject("sphere.glb"));
		newModel->GetTransform().SetLocalPosition({ (i - ((SPHERE_COUNT - 1) / 2.0f)) * SPHERE_SEPARATION, 0, 0 });
		auto mesh = newModel->GetChildrenByType<URM::Engine::MeshObject>(true)[0];

		auto iValue = (i / ((float)SPHERE_COUNT - 1));
		auto exponentX = std::pow(iValue, 2);

		auto simpleMaterial = std::shared_ptr<URM::Core::MaterialSimple>(new URM::Core::MaterialSimple(engine.GetCore()));
		simpleMaterial->data.roughnessPowerExponent = MIN_ROUGHNESS + exponentX * (MAX_ROUGHNESS - MIN_ROUGHNESS);
		simpleMaterial->data.albedoColor = Color(1.0f, 0.0f, 0.0f);
		mesh->material = simpleMaterial;
	}

	auto camera = engine.GetScene().GetMainCamera().lock();
	camera->GetTransform().SetPosition({ 0, 0, SPHERE_SEPARATION * SPHERE_COUNT / 2.0f });
	camera->GetTransform().LookAt({ 0, 0, 0 });
}

void MaterialTest::OnUpdate(URM::Engine::Engine& engine) {
	const float distance = this->mLight->GetTransform().GetPosition().z;
	auto alpha = engine.GetTimer().GetElapsedTime() * 3.14f;

	this->mLight->GetTransform().SetLocalPosition({
		cos(alpha) * distance / 2.0f,
		sin(alpha) * distance / 2.0f,
		distance,
	});
}
