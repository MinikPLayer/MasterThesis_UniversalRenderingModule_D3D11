//
// Main.cpp
//

#include "pch.h"
#include "Game.h"
#include <spdlog/spdlog.h>

#include "Window.h"
#include <Log.h>
#include <D3DEngine.h>

using namespace DirectX;

#ifdef __clang__
#pragma clang diagnostic ignored "-Wcovered-switch-default"
#pragma clang diagnostic ignored "-Wswitch-enum"
#endif

// Indicates to hybrid graphics systems to prefer the discrete part by default
extern "C"
{
    __declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
    __declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}

void Draw(D3DEngine& engine) {
	engine.Clear(Colors::Black);
    engine.Present(0);
}

// Entry point
int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
	UNREFERENCED_PARAMETER(nCmdShow);

    if (!XMVerifyCPUSupport())
        return 1;

    HRESULT hr = CoInitializeEx(nullptr, COINITBASE_MULTITHREADED);
    if (FAILED(hr))
        return 1;

    Logger::InitLogger();
    
    std::vector<std::unique_ptr<D3DEngine>> engines;
    for (int i = 0; i < 10; i++) {
		engines.push_back(std::make_unique<D3DEngine>(WindowCreationParams(1280, 720, "UniversalRenderingModule", hInstance)));
    }

    //D3DEngine engine(WindowCreationParams(1280, 720, "UniversalRenderingModule", hInstance));
    //D3DEngine engine2(WindowCreationParams(800, 600, "UniversalRenderingModule", hInstance));

    for (auto& engine : engines) {
        engine->OnWindowPaint = [&](D3DEngine& engine) {
            Draw(engine);
        };
    }


    while(engines.size() > 0) {
        for(auto i = 0; i < engines.size(); i++) {
			auto* engine = engines[i].get();
            if (engine->GetWindow().IsDestroyed()) {
				engines.erase(engines.begin() + i);
				i--;
                continue;
            }

            engine->GetWindow().PollEvents();
            Draw(*engine);
        }
    }
    Logger::DisposeLogger();

    return 0;
}