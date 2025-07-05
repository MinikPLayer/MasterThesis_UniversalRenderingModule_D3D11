#include <Windows.h>
#include <URM/Engine/Engine.h>
#include <URM/Core/Log.h>

#include "VertexThroughputTest.h"
#include "SponzaTest.h"
#include "LightsTest.h"

using namespace URM;

auto* test = new VertexThroughputTest();

void OnUpdate(Engine::Engine& engine) {
	test->Update(engine);
}

int WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow) 
{
	Engine::Engine engine(test->GetWindowParams(hInstance));
	engine.GetScene().SetMainCamera(engine.GetScene().GetRoot().lock()->AddChild(new Engine::CameraObject()));

	test->Init(engine);
	engine.onUpdate = OnUpdate;

	engine.RunLoop();
	return 0;
}