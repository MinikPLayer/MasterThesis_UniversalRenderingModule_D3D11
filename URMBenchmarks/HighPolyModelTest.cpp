#include "HighPolyModelTest.h"

void HighPolyModelTest::OnInit(URM::Engine::Engine& engine) {
	auto root = engine.GetScene().GetRoot().lock();

	auto light = new URM::Engine::LightObject();
	light->GetTransform().SetPosition({ 0, 3, 5 });
	root->AddChild(light);
	mModelsObject = root->AddChild(new URM::Engine::SceneObject());
	auto newModel = mModelsObject->AddChild(new URM::Engine::ModelObject("roman_marble_3.000.000_triangles.glb", mUsePBR));

	//model->GetTransform().SetLocalPosition({ -modelSize.x / 2, -modelSize.y / 2, -modelSize.z / 2 });
	mModelsObject->GetTransform().SetPosition({ 0, -0.5, 0 });

	auto camera = engine.GetScene().GetMainCamera().lock();
	camera->GetTransform().SetPosition({ 0, 0, 1.2 });
	camera->GetTransform().LookAt({ 0, 0, 0 });
}

void HighPolyModelTest::OnUpdate(URM::Engine::Engine& engine) {
	mModelsObject->GetTransform().SetLocalRotation({
		0.0f,
		engine.GetTimer().GetElapsedTime() * 90.0f,
		0.0f
	});
}
