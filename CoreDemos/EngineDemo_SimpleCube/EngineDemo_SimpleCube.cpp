#include <URM/Engine/Engine.h>
#include <URM/Engine/CameraObject.h>
#include <URM/Engine/ModelObject.h>
#include <URM/Core/StandardMaterials.h>

using namespace URM::Core;
using namespace URM::Engine;

struct NormalColorMaterial : public Material {
    const wchar_t* GetShaderFilePath() const override {
        return L"NormalColorPixelShader.cso";
	}
};

int WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow) {
	// Utworzenie instancji silnika z parametrami okna
    Engine engine(
        WindowCreationParams(800, 600, "Engine Demo - Cube", hInstance)
    );
	// Ustawienie kolor tła
	engine.renderParameters.clearColor = Color(0.3f, 0.5f, 1.0f);

	// Utworzenie głównej kamery wraz z pozycją i kierunkiem
    auto camera = std::make_shared<CameraObject>(45.0f);
	camera->GetTransform().SetPosition({ 4.0f, 4.0f, 4.0f });
	camera->GetTransform().LookAt({ 0.0f, 0.0f, 0.0f });
	// Ustawienie kamery jako głównej kamery w scenie
	engine.GetScene().SetMainCamera(camera);
    engine.GetScene().GetRoot().lock()->AddChild(camera);

	// Dodanie punktu światła do sceny
    auto newLight = engine.GetScene().GetRoot().lock()->AddChild(new LightObject());
	newLight->GetTransform().SetPosition({ -4.0f, 1.2f, 0.0f });

	// Utworzenie obiektu z niestandardowym materiałem / shader'em
	auto normalColorMaterial = std::shared_ptr<NormalColorMaterial>(
		new NormalColorMaterial()
	);
    auto cube = engine.GetScene().GetRoot().lock()->AddChild(
		new ModelObject("cube.glb", normalColorMaterial)
	);
	cube->GetTransform().SetLocalPosition({ 0.0f, 0.0f, 0.0f });

	// Utworzenie obiektu z materiałem prostym o czerwonym kolorze
	auto standardMaterial = std::shared_ptr<MaterialSimple>(
		new MaterialSimple(engine.GetCore(), MaterialSimpleData(128, Color(1, 0, 0, 1)))
	);
    auto cube2 = engine.GetScene().GetRoot().lock()->AddChild(
		new ModelObject("cube.glb", standardMaterial)
	);
	cube2->GetTransform().SetLocalPosition({ -4.0f, 0.0f, 0.0f });

	// Utworzenie obiektu z domyślnym materiałem
    auto cube3 = engine.GetScene().GetRoot().lock()->AddChild(
		new ModelObject("cube.glb")
	);
    cube3->GetTransform().SetLocalPosition({ 0.0f, 0.0f, -4.0f });

	// Uruchomienie pętli silnika
    engine.RunLoop();
}