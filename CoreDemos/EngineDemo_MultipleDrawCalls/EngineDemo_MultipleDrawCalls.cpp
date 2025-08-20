#include <URM/Engine/Engine.h>
#include <URM/Engine/CameraObject.h>
#include <URM/Engine/ModelObject.h>
#include <URM/Core/StandardMaterials.h>
#include <vector>

using namespace URM::Core;
using namespace URM::Engine;

int WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow) {
    // Utworzenie instancji silnika z parametrami okna
    Engine engine(
        WindowCreationParams(800, 600, "Engine Demo - Multiple draw calls", hInstance)
    );

    // Przygotowanie pierwszej sceny
    auto camera = engine.GetScene().GetRoot().lock()->AddChild(new CameraObject(45.0f));
    camera->GetTransform().SetPosition({ 2.0f, 4.0f, 8.0f });
    camera->GetTransform().LookAt({ 2.0f, 0.0f, 0.0f });
    engine.GetScene().SetMainCamera(camera);
    auto light = engine.GetScene().GetRoot().lock()->AddChild(new LightObject());
    light->GetTransform().SetPosition({ 2.0f, 3.0f, 4.0f });
    auto model = engine.GetScene().GetRoot().lock()->AddChild(new ModelObject("cube.glb"));

    // Przygotowanie drugiej sceny
    auto secondScene = std::make_shared<Scene>(engine.GetCore());
    auto secondCamera = secondScene->GetRoot().lock()->AddChild(new CameraObject(90.0f));
    secondCamera->GetTransform().SetPosition({ -2.0f, 4.0f, 4.0f });
	secondCamera->GetTransform().LookAt({ -2.0f, 0.0f, 0.0f });
	secondScene->SetMainCamera(secondCamera);
	auto secondLight = secondScene->GetRoot().lock()->AddChild(new LightObject());
	secondLight->color = Color(1.0f, 0.0f, 0.0f);
	secondLight->GetTransform().SetPosition({ -0.5f, 1.5f, 1.1f });
    secondScene->GetRoot().lock()->AddChild(new ModelObject("cube.glb"));

	// Przygotowanie trzeciej kamery i światła
	auto thirdCamera = std::make_shared<CameraObject>(15.0f);
	thirdCamera->GetTransform().SetPosition({ 4.0f, 8.0f, -16.0f });
	thirdCamera->GetTransform().LookAt({ 0.0f, 0.0f, 0.0f });
    auto thirdLight = std::make_shared<LightObject>();
	thirdLight->GetTransform().SetPosition({ 1.5f, 2.0f, -3.0f });
	thirdLight->color = Color(0.0f, 1.0f, 0.0f);
	auto thirdDrawLights = std::vector<std::weak_ptr<LightObject>>{ thirdLight };

    while (!engine.ShouldClose()) {
        engine.Update();
        engine.Clear(Color(0.3f, 0.5f, 1.0f));
        engine.Draw(engine.renderParameters); // Normalne rysowanie sceny przypisanej do silnika
		engine.Draw(engine.renderParameters, *secondScene); // Rysowanie drugiej sceny
		// Rysowanie modeli z pierwszej sceny, ale z użyciem innej kamery i światła
        engine.Draw(engine.renderParameters, thirdCamera, engine.GetScene().GetMeshes(), thirdDrawLights);
        engine.Present(0);
    }
}