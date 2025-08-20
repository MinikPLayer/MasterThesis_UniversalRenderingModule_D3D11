#include <URM/Engine/Engine.h>
#include <URM/Engine/CameraObject.h>
#include <URM/Engine/ModelObject.h>

using namespace URM::Core;
using namespace URM::Engine;

int WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow) {
    // Utworzenie instancji silnika z parametrami okna
    Engine engine(
        WindowCreationParams(800, 600, "Engine Demo - Cube", hInstance)
    );

    // Przygotowanie sceny
    auto camera = engine.GetScene().GetRoot().lock()->AddChild(new CameraObject(45.0f));
    camera->GetTransform().SetPosition({ 4.0f, 4.0f, 4.0f });
    camera->GetTransform().LookAt({ 0.0f, 0.0f, 0.0f });
    engine.GetScene().SetMainCamera(camera);
    auto light = engine.GetScene().GetRoot().lock()->AddChild(new LightObject());
    light->GetTransform().SetPosition({ 2.0f, 3.0f, 4.0f });
    auto model = engine.GetScene().GetRoot().lock()->AddChild(new ModelObject("cube.glb"));

    // Pobranie uchwytu do klawiatury
    auto keyboard = engine.GetScene().GetCore().GetKeyboard();

    float rotation = 0.0f;
    // Ręczna pętla rysowania
    while (!engine.ShouldClose()) {
        // Aktualizacja zdarzeń
        engine.Update();
        // Rysowanie sceny tylko jeśli klawisz 'R' jest wciśnięty
        if (keyboard.lock()->GetState().IsKeyDown(DirectX::Keyboard::R)) {
            rotation += engine.GetTimer().GetDeltaTime() * 90.0f;
            model->GetTransform().SetLocalRotation({ 0.0f, rotation, 0.0f });

            // Ręczne czyszczenie, rysowanie i prezentacja
            engine.Clear(Color(0.3f, 0.5f, 1.0f));
            engine.Draw(engine.renderParameters);
            engine.Present(0);
        }
    }
}