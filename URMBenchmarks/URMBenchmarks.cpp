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

ITest* test;

// TESTS
//auto* test = new LightsTest(true, false);
//auto* test = new MultipleObjectsTest();
//auto* test = new ObjectsTransformThroughputTest();
//auto* test = new VertexThroughputTest();

// DEMOS
//auto* test = new HighPolyModelTest(true);
//auto* test = new MaterialTest();
//auto* test = new SponzaTest(true);
//auto* test = new SponzaTest(false);

std::unordered_map<std::string, std::function<ITest*()>> tests = {
	{ "test_lights", [] { return new LightsTest(true, false); } },
	{ "test_objects", [] { return new MultipleObjectsTest(); } },
	{ "test_transform", [] { return new ObjectsTransformThroughputTest(); } },
	{ "test_vertex", [] { return new VertexThroughputTest(); } },

	{ "demo_lights", [] { return new LightsTest(false, true); }},
	{ "demo_highPoly", [] { return new HighPolyModelTest(false); }},
	{ "demo_highPoly_PBR", [] { return new HighPolyModelTest(true); }},
	{ "demo_material", [] { return new MaterialTest(); }},
	{ "demo_sponza", [] { return new SponzaTest(false); }},
	{ "demo_sponza_PBR", [] { return new SponzaTest(true); }},
};

void OnUpdate(Engine::Engine& engine) {
	test->Update(engine);
}

bool SetupTest() {
	int argCount = __argc;
	char** args = __argv;

	for (int i = 1; i < argCount; i++) {
		auto stringArg = std::string(args[i]);
		for (auto& testEntry : tests) {
			if (testEntry.first == stringArg) {
				test = tests.at(stringArg)();
				return true;
			}
		}
	}

	std::stringstream ss;
	ss << "Usage: URMBenchmark.exe {test}\n";
	ss << "Available tests: \n";
	for (auto& testEntry : tests) {
		ss << fmt::format("\ - {}", testEntry.first) << "\n";
	}

	spdlog::info(ss.str());
	MessageBox(nullptr, URM::Core::StringUtils::StringToWString(ss.str()).c_str(), L"No test provided", MB_OK | MB_ICONERROR);
	return false;
}

int WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow) 
{
	if (!SetupTest()) {
		return 1;
	}

	Engine::Engine engine(test->GetWindowParams(hInstance));
	engine.GetScene().SetMainCamera(engine.GetScene().GetRoot().lock()->AddChild(new Engine::FlyCameraObject()));

	test->Init(engine);
	engine.onUpdate = OnUpdate;

	engine.RunLoop();
	return 0;
}