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

std::chrono::high_resolution_clock::time_point lastTime = std::chrono::high_resolution_clock::now();
std::chrono::high_resolution_clock::time_point programStartTime = std::chrono::high_resolution_clock::now();
void Draw(D3DEngine& engine) {
	auto now = std::chrono::high_resolution_clock::now();
	auto deltaTime = std::chrono::duration_cast<std::chrono::microseconds>(now - lastTime).count() / 1000000.0f;
	lastTime = now;

	auto elapsedTime = std::chrono::duration_cast<std::chrono::microseconds>(now - programStartTime).count() / 1000000.0f;

    engine.Clear(DirectX::XMVECTORF32{ sin(elapsedTime / 2.0f) / 2.0f + 0.5f, cos(elapsedTime / 3.f) / 2.0f + 0.5f, 0.0f, 1.0f});
    engine.Present(1);
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
    

    D3DEngine engine(WindowCreationParams(1600, 1000, "UniversalRenderingModule", hInstance));
    engine.OnWindowPaint = [&](D3DEngine& engine) {
        Draw(engine);
    };

    while(!engine.GetWindow().IsDestroyed()) {
        engine.GetWindow().PollEvents();
        Draw(engine);
    }
    Logger::DisposeLogger();

    return 0;
}