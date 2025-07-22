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
#include <iostream>

#include <fstream>

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

	// Path, command, argument
	if (argCount == 3) {
		auto stringArg = std::string(args[1]);
		auto parameter = std::string(args[2]);
		if (stringArg == "save_tests_list_to_file") {
			auto file = std::ofstream(parameter);
			if (!file.good()) {
				spdlog::error("Cannot open file to save tests list at {}", parameter);
				return false;
			}

			for (auto& t : tests) {
				file << t.first << "\n";
			}
			file.close();

			spdlog::info("Tests saved to {}\n", parameter);

			return false;
		}
		else if (stringArg == "run") {
			for (auto& testEntry : tests) {
				if (testEntry.first == parameter) {
					test = tests.at(parameter)();
					return true;
				}
			}
		}
	}

	std::stringstream ss;
	ss << "Usage: URMBenchmark.exe {run / save_tests_list_to_file} {test / file_path}\n";
	ss << "Available tests: \n";
	for (auto& testEntry : tests) {
		ss << fmt::format("\ - {}", testEntry.first) << std::endl;
	}

	spdlog::info(ss.str());
	MessageBox(nullptr, URM::Core::StringUtils::StringToWString(ss.str()).c_str(), L"No test provided", MB_OK | MB_ICONERROR);

	return false;
}

int WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow) 
{
	URM::Core::Logger::InitLogger();

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