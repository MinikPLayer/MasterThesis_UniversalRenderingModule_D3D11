#include <URM/Engine/Engine.h>
#include <URM/Engine/CameraObject.h>
#include <URM/Engine/ModelObject.h>
#include <URM/Core/StandardMaterials.h>
#include <URM/Core/StandardVertexTypes.h>
#include <URM/Core/D3DInputLayout.h>


int WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow) {
	// Utworzenie instancji silnika z parametrami okna
    URM::Engine::Engine engine(
        URM::Core::WindowCreationParams(800, 600, "Engine Demo - Cube", hInstance)
    );
	// Ustawienie kolor tła
	engine.renderParameters.clearColor = Color(0.3f, 0.5f, 1.0f);

	// Utworzenie głównej kamery wraz z pozycją i kierunkiem
    auto camera = std::make_shared< URM::Engine::CameraObject>(45.0f);
	camera->GetTransform().SetPosition({ 4.0f, 4.0f, 4.0f });
	camera->GetTransform().LookAt({ 0.0f, 0.0f, 0.0f });
	// Ustawienie kamery jako głównej kamery w scenie
	engine.GetScene().SetMainCamera(camera);
    engine.GetScene().GetRoot().lock()->AddChild(camera);

	// Dodanie punktu światła do sceny
    auto newLight = engine.GetScene().GetRoot().lock()->AddChild(new  URM::Engine::LightObject());
	newLight->GetTransform().SetPosition({ 1.0f, 1.2f, 1.5f });

	// Utworzenie obiektu z niestandardowym materiałem / shader'em
    auto cube = engine.GetScene().GetRoot().lock()->AddChild(new  URM::Engine::ModelObject("cube.glb"));

	// Utworzenie obiektów shader'ów
	auto customVertexShader = new URM::Core::VertexShader(engine.GetCore(), L"RedVertexShader.cso");
	auto customPixelShader = new URM::Core::PixelShader(engine.GetCore(), L"RedPixelShader.cso");

	// Utworzenie bufora wierzchołków z danymi wierzchołków
	auto customVertexData = std::vector<URM::Core::VertexPosition>{
		{ 0.9, -0.4, 0.0 },
		{ 0.9, -0.9, 0.0 },
		{ 0.4, -0.9, 0.0 },
	};

	// Utworzenie bufora wierzchołków i układu wejściowego
	auto customVertexBuffer = URM::Core::D3DVertexBuffer<URM::Core::VertexPosition>::Create(engine.GetCore(), customVertexData);
	auto customInputLayout = URM::Core::D3DInputLayout<URM::Core::VertexPosition>(engine.GetCore(), *customVertexShader);

	// Uruchomienie pętli silnika
	while (!engine.ShouldClose()) {
		// Zwykłe rysowanie sceny
		engine.Update();
		engine.Clear();
		engine.Draw(engine.renderParameters, engine.GetScene());

		// Rysowanie przy pomocy elementów pakietu Core,
		// z pominięciem silnika
		customInputLayout.Bind(engine.GetCore());
		customVertexBuffer.Bind(engine.GetCore(), 0);
		customVertexShader->Bind(engine.GetCore());
		customPixelShader->Bind(engine.GetCore());
		engine.GetCore().GetContext()->Draw(customVertexData.size(), 0);

		engine.Present(engine.vSyncInterval);
	}
}