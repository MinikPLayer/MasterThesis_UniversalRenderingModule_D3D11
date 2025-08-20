#include <URM/Core/D3DCore.h>

int WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow) {
    URM::Core::D3DCore core(URM::Core::WindowCreationParams(800, 600, "Empty window", hInstance));
    while(!core.GetWindow().IsDestroyed()) {
        core.GetWindow().PollEvents();
        core.ClearFramebuffer(DirectX::SimpleMath::Color(0.3, 0.5, 1.0));
        core.Present(0);
	}
}