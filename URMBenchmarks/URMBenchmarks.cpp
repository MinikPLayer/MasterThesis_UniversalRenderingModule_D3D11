#include <Windows.h>
#include <URM/Engine/Engine.h>
#include <URM/Core/Log.h>
#include <URM/Engine/FlyCameraObject.h>

#include "VertexThroughputTest.h"
#include "PixelThroughputTest.h"
#include "SponzaTest.h"
#include "LightsTest.h"
#include "MultipleObjectsTest.h"
#include "ObjectsTransformThroughputTest.h"
#include "HighPolyModelTest.h"
#include "MaterialTest.h"

using namespace URM;

auto* test = new HighPolyModelTest(true);

void OnUpdate(Engine::Engine& engine) {
	test->Update(engine);
}

int WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow) 
{
	Engine::Engine engine(test->GetWindowParams(hInstance));
	engine.GetScene().SetMainCamera(engine.GetScene().GetRoot().lock()->AddChild(new Engine::FlyCameraObject()));

	test->Init(engine);
	engine.onUpdate = OnUpdate;

	engine.RunLoop();
	return 0;
}