#include <URM/Engine/Engine.h>
#include <URM/Engine/FlyCameraObject.h>
#include <URM/Engine/ModelObject.h>

using namespace DirectX;

struct NormalColorMaterial : public URM::Core::Material {
    const wchar_t* GetShaderFilePath() const override {
        return L"NormalColorPixelShader.cso";
	}
};

int WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow) {
    URM::Engine::Engine engine(
        URM::Core::WindowCreationParams(800, 600, "Engine Demo - Cube", hInstance)
    );
	engine.renderParameters.clearColor = DirectX::SimpleMath::Color(0.3f, 0.5f, 1.0f);

    auto camera = std::make_shared<URM::Engine::CameraObject>();
	camera->GetTransform().SetPosition({ 4.0f, 4.0f, 4.0f });
	camera->GetTransform().LookAt({ 0.0f, 0.0f, 0.0f });
	engine.GetScene().SetMainCamera(camera);
    engine.GetScene().GetRoot().lock()->AddChild(camera);

    auto newLight = engine.GetScene().GetRoot().lock()->AddChild(new URM::Engine::LightObject());

	auto normalColorMaterial = std::shared_ptr<NormalColorMaterial>(new NormalColorMaterial());
    auto cube = std::make_shared<URM::Engine::ModelObject>("cube.glb", normalColorMaterial);
	cube->GetTransform().SetLocalPosition({ 0.0f, 0.0f, 0.0f });
    engine.GetScene().GetRoot().lock()->AddChild(cube);


    engine.RunLoop();
}