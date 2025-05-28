#include <spdlog/spdlog.h>

#include <URM/Core/pch.h>
#include <URM/Core/Window.h>
#include <URM/Core/Log.h>
#include <URM/Core/D3DCore.h>
#include <URM/Core/Mesh.h>
#include <URM/Core/StandardVertexTypes.h>
#include <URM/Core/ID3DBuffer.h>
#include <URM/Core/ShaderProgram.h>
#include <URM/Core/D3DInputLayout.h>

#include <URM/Core/D3DViewport.h>
#include <URM/Core/D3DConstantBuffer.h>
#include <URM/Core/D3DRasterizerState.h>
#include <URM/Core/ModelLoader.h>
#include <URM/Scene/Scene.h>

#include <URM/Scene/SceneModel.h>
#include <URM/Scene/SceneMesh.h>

#include <URM/Engine/Engine.h>
#include <thread>

using namespace DirectX;

// Notes:
// - Ograniczenia: 
//      - Jeden silnik = jedno okno. 

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
void Clear(URM::Core::D3DCore& core) {
    auto now = std::chrono::high_resolution_clock::now();
    auto deltaTime = std::chrono::duration_cast<std::chrono::microseconds>(now - lastTime).count() / 1000000.0f;
    UNREFERENCED_PARAMETER(deltaTime);
    lastTime = now;

    auto elapsedTime = std::chrono::duration_cast<std::chrono::microseconds>(now - programStartTime).count() / 1000000.0f;

    core.Clear(DirectX::Colors::Black);
}

#pragma region ConstantBufferTest

struct VertexConstantBuffer
{
    DirectX::XMMATRIX WVP;
	DirectX::XMMATRIX worldMatrix;
	DirectX::XMMATRIX inverseWorldMatrix;
};

struct PixelConstantBuffer {
    Vector4 viewPosition;

    struct Light {
        Vector4 color;
        Vector4 position;
        float ambientIntensity;
        float diffuseIntensity;
        float specularIntensity;

        int __padding__;

        Light(float r, float g, float b,
            float x, float y, float z,
            float ambient, float diffuse, float specular)
            : color(r, g, b, 1.0f), position(x, y, z, 1.0f), ambientIntensity(ambient), diffuseIntensity(diffuse), specularIntensity(specular)
        {
        }
    } light;

    struct Material {
        int useAlbedoTexture = 0;
        Vector3 __padding__;
    } material;

    PixelConstantBuffer(Light light, Vector3 viewPos) : light(light), viewPosition(viewPos.x, viewPos.y, viewPos.z, 1.0f) {}
};

struct WVPMatrix {
    DirectX::XMMATRIX WVP;
    DirectX::XMMATRIX World;

    void Apply(VertexConstantBuffer& buffer) {
        buffer.WVP = DirectX::XMMatrixTranspose(WVP);
		buffer.worldMatrix = DirectX::XMMatrixTranspose(World);
		buffer.inverseWorldMatrix = DirectX::XMMatrixInverse(nullptr, World);
    }

	WVPMatrix(DirectX::XMMATRIX projection, DirectX::XMMATRIX view, DirectX::XMMATRIX world) : World(world) {
		WVP = world * view * projection;
	}
};

WVPMatrix CreateTransformationMatrix(
    const DirectX::XMFLOAT3& modelPosition,
    const DirectX::XMFLOAT3& modelRotationAngles, // Degrees
    const DirectX::XMFLOAT3& modelScale,
    const DirectX::XMFLOAT3& cameraPosition,
    const DirectX::XMFLOAT3& cameraTarget,
    const DirectX::XMFLOAT3& cameraUpDirection,
    float fov, // Degrees
    float nearPlane,
    float farPlane,
    URM::Core::Size2i windowSize
)
{
    // 1. World Matrix
    DirectX::XMMATRIX matScale = DirectX::XMMatrixScaling(modelScale.x, modelScale.y, modelScale.z);
    DirectX::XMMATRIX matRotation = DirectX::XMMatrixRotationRollPitchYaw(
        DirectX::XMConvertToRadians(modelRotationAngles.x),
        DirectX::XMConvertToRadians(modelRotationAngles.y),
        DirectX::XMConvertToRadians(modelRotationAngles.z)
    );
    DirectX::XMMATRIX matTranslation = DirectX::XMMatrixTranslation(modelPosition.x, modelPosition.y, modelPosition.z);
    DirectX::XMMATRIX matWorld = matScale * matRotation * matTranslation;

    // 2. View Matrix
    auto vecCameraPosition = DirectX::XMLoadFloat3(&cameraPosition);
    auto vecCameraTarget = DirectX::XMLoadFloat3(&cameraTarget);
    auto vecCameraUp = DirectX::XMLoadFloat3(&cameraUpDirection);
    auto matView = DirectX::XMMatrixLookAtLH(vecCameraPosition, vecCameraTarget, vecCameraUp);

    // 3. Projection Matrix (Orthographic)
    DirectX::XMMATRIX matProjection = DirectX::XMMatrixPerspectiveFovLH(
        DirectX::XMConvertToRadians(fov),
        windowSize.width / (float)windowSize.height,
        nearPlane,
        farPlane
    );

    return WVPMatrix(matProjection, matView, matWorld);
}

struct TestDrawData {
    URM::Core::D3DCore& core;
    URM::Core::D3DConstantBuffer& vertexConstantBuffer;
	URM::Core::D3DConstantBuffer& pixelConstantBuffer;
    URM::Core::D3DViewport& viewport;
    URM::Core::D3DRasterizerState& rState;
    URM::Core::D3DSampler& sampler;
    URM::Scene::Scene& scene;
};

void TestDrawMesh(TestDrawData& data, PixelConstantBuffer pcb, std::weak_ptr<URM::Scene::SceneMesh> mesh, WVPMatrix transformMatrix) {
    auto nodeWorldMatrix = mesh.lock()->GetTransform().GetWorldSpaceMatrix();
    VertexConstantBuffer cBufferData;
	transformMatrix.World = nodeWorldMatrix * transformMatrix.World;
	transformMatrix.WVP = nodeWorldMatrix * transformMatrix.WVP;
    transformMatrix.Apply(cBufferData);
    data.vertexConstantBuffer.UpdateWithData(data.core, &cBufferData);
   
    auto sceneMesh = mesh.lock();
    sceneMesh->GetInputLayout()->Bind(data.core);
    sceneMesh->GetShader()->Bind(data.core);

    auto m = mesh.lock()->GetMesh();
    m.GetVertexBuffer().Bind(data.core, 0);
    
    if (m.ContainsTextures()) {
        m.BindTextures(data.core);
        pcb.material.useAlbedoTexture = 1;
    }
    else {
        pcb.material.useAlbedoTexture = 0;
    }
    data.pixelConstantBuffer.UpdateWithData(data.core, &pcb);
    
    if (m.ContainsIndices()) {
        m.GetIndexBuffer().Bind(data.core, 0);
    
        data.core.GetContext()->DrawIndexed(m.GetIndicesCount(), 0, 0);
    }
    else {
    	m.GetVertexBuffer().Bind(data.core, 0);
        data.core.GetContext()->Draw(m.GetVerticesCount(), 0);
    }
}

static int cbCounter = 0;
static const DirectX::XMFLOAT3 camPos = { 0.0f, 4.0f, -8.0f };
static const DirectX::XMFLOAT3 camTarget = { 0.0f, 0.0f, 0.0f };
static const DirectX::XMFLOAT3 camUp = { 0.0f, 1.0f, 0.0f };
WVPMatrix TestDrawCreateWVP(XMFLOAT3 positionOffset, URM::Core::Size2i windowSize, float rotation, float scale = 1.0f) {
    DirectX::XMFLOAT3 modelPos = positionOffset;
    DirectX::XMFLOAT3 modelRot = { 0.0f, rotation, 0.0f };
    DirectX::XMFLOAT3 modelScl = { scale, scale, scale };

    float fov = 45.f;
    float nearPlane = 1.0f;
    float farPlane = 100.0f;

    auto WVP = CreateTransformationMatrix(
        modelPos,
        modelRot,
        modelScl,
        camPos,
        camTarget,
        camUp,
        fov,
        nearPlane,
        farPlane,
        windowSize
    );

    return WVP;
}

template<URM::Core::VertexTypeConcept V>
void TestDraw(TestDrawData data) {
    auto context = data.core.GetContext();

    // Aktualizacja stałej buforowej
	auto elapsedTime = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - programStartTime).count() / 1000000.0f;

    auto vp = data.viewport.GetData();
    vp.size = data.core.GetWindow().GetSize();
    data.viewport.SetData(vp);
    data.viewport.Bind(data.core);

    data.rState.Bind(data.core);

    data.core.SetPrimitiveTopology(URM::Core::PrimitiveTopologies::TRIANGLE_LIST);

    data.vertexConstantBuffer.Bind(data.core, 0);
	data.pixelConstantBuffer.Bind(data.core, 1);

    // Bind the default sampler
    data.sampler.Bind(data.core, 0);

	auto windowSize = data.core.GetWindow().GetSize();
	auto rotation = elapsedTime * 90.0f;
	auto rotationRad = rotation * XM_PI / 180.0f;

    const float lightDistance = 2.1f;
	auto lightPosition = Vector3(
		sin(rotationRad) * lightDistance,
		lightDistance / 1.5f,
		cos(rotationRad) * lightDistance
	);
    auto pixelBufferValue = PixelConstantBuffer(
        PixelConstantBuffer::Light(
            1.0f, 1.0f, 1.0f,
			lightPosition.x, lightPosition.y, lightPosition.z,
            0.05f, 0.9f, 1.0f
        ),
        camPos
    );
	data.pixelConstantBuffer.UpdateWithData(data.core, &pixelBufferValue);

    auto WVP = TestDrawCreateWVP({ 0.0f, 0.0f, 0.0f }, windowSize, 0);
    for (auto& mesh : data.scene.GetMeshes()) {
		TestDrawMesh(data, pixelBufferValue, mesh, WVP);
    }

    data.core.Present(0);
}

#pragma endregion

const bool ENGINE_MODE = true;
const bool ENGINE_LOOP_MODE = false;

void FillScene(URM::Core::D3DCore& core, URM::Scene::Scene& scene) {
    auto alternativeShader = URM::Core::ShaderProgram(core, L"SimpleVertexShader.cso", L"ColorOnlyPixelShader.cso");
    auto alternativeLayout = URM::Core::ModelLoaderLayout(core, alternativeShader);

    auto suzanne = new URM::Scene::SceneModel(
        "suzanne.glb", 
        std::make_shared<URM::Core::ShaderProgram>(alternativeShader), 
        std::make_shared< URM::Core::ModelLoaderLayout>(alternativeLayout)
    );
    scene.GetRoot().lock()->AddChild(suzanne)->GetTransform().SetLocalPosition({ -2.0f, 0.0f, 0.0f });

    auto cube = new URM::Scene::SceneModel("cube_textured.glb");
    scene.GetRoot().lock()->AddChild(cube)->GetTransform().SetLocalPosition({ 2.0f, 0.0f, 0.0f });
}

int actualMainEngine(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow) {
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
    UNREFERENCED_PARAMETER(nCmdShow);

    if (!XMVerifyCPUSupport())
        return 1;

    HRESULT hr = CoInitializeEx(nullptr, COINITBASE_MULTITHREADED);
    if (FAILED(hr))
        return 1;

    URM::Engine::Engine engine(URM::Core::WindowCreationParams(1600, 1000, "UniversalRenderingModule", hInstance));
    auto& scene = engine.GetScene();
    FillScene(engine.GetCore(), scene);

    if (ENGINE_LOOP_MODE) {
        engine.RunLoop();
    }
    else {
        float deltaCounter = 0;
        const float drawInterval = 1.0;
        while (!engine.ShouldClose()) {
            engine.Update();

            auto deltaTime = engine.GetTimer().GetDeltaTime();
            deltaCounter += engine.GetTimer().GetDeltaTime();
            if (deltaCounter > drawInterval) {
                engine.Clear();
                engine.Draw(engine.RenderParameters, scene.GetMeshes());
                engine.Present(0);
                
                deltaCounter -= drawInterval;
            }
        }
    }


    return 0;
}

int actualMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow) {
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
    UNREFERENCED_PARAMETER(nCmdShow);

    if (!XMVerifyCPUSupport())
        return 1;

    HRESULT hr = CoInitializeEx(nullptr, COINITBASE_MULTITHREADED);
    if (FAILED(hr))
        return 1;

    URM::Core::D3DCore core(URM::Core::WindowCreationParams(1600, 1000, "UniversalRenderingModule", hInstance));
    URM::Scene::Scene scene(core);
    FillScene(core, scene);

    URM::Core::D3DConstantBuffer vertexConstantBuffer = URM::Core::D3DConstantBuffer::Create<VertexConstantBuffer>(core, URM::Core::ShaderStages::VERTEX);
    URM::Core::D3DConstantBuffer pixelConstantBuffer = URM::Core::D3DConstantBuffer::Create<PixelConstantBuffer>(core, URM::Core::ShaderStages::PIXEL);
    URM::Core::D3DViewport viewport(URM::Core::D3DViewportData(core.GetWindow().GetSize()));

    auto rStateData = URM::Core::D3DRasterizerStateData();
    rStateData.cullMode = URM::Core::CullModes::BACK;
    auto rState = URM::Core::D3DRasterizerState(rStateData);

    auto sampler = URM::Core::D3DSampler(URM::Core::D3DSamplerData());

    auto testDrawData = TestDrawData {
        core,
        vertexConstantBuffer,
        pixelConstantBuffer,
        viewport,
        rState,
        sampler,
        scene
    };

    core.OnWindowPaint = [&](URM::Core::D3DCore& core) {
        Clear(core);
        TestDraw<URM::Core::VertexPositionColor>(testDrawData);
    };

    while (!core.GetWindow().IsDestroyed()) {
        core.GetWindow().PollEvents();
        Clear(core);
        TestDraw<URM::Core::VertexPositionColor>(testDrawData);
    }
    URM::Core::Logger::DisposeLogger();

    return 0;
}

std::string GetLastErrorAsString()
{
    //Get the error message ID, if any.
    DWORD errorMessageID = ::GetLastError();
    if (errorMessageID == 0) {
        return std::string(); //No error message has been recorded
    }

    LPSTR messageBuffer = nullptr;

    //Ask Win32 to give us the string version of that message ID.
    //The parameters we pass in, tell Win32 to create the buffer that holds the message for us (because we don't yet know how long the message string will be).
    size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL, errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);

    //Copy the error message into a std::string.
    std::string message(messageBuffer, size);

    //Free the Win32's string's buffer.
    LocalFree(messageBuffer);

    return message;
}

// Entry point
int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
    URM::Core::Logger::InitLogger();
    int returnCode = 123456;
    try {
        returnCode = ENGINE_MODE ? 
            actualMainEngine(hInstance, hPrevInstance, lpCmdLine, nCmdShow) :
            actualMain(hInstance, hPrevInstance, lpCmdLine, nCmdShow);
    }
    catch (std::exception e) {
        spdlog::critical("Exception: {}", e.what());

        std::wstring msgBoxMessage = L"Unrecoverable error: \n" + std::wstring(e.what(), e.what() + strlen(e.what()));
        if (GetLastError() != 0) {
            auto lastWinApiErrorString = GetLastErrorAsString();
            msgBoxMessage += L"\n\nLast WinAPI error: " + std::wstring(lastWinApiErrorString.begin(), lastWinApiErrorString.end());
        }

        MessageBox(nullptr, std::wstring(e.what(), e.what() + strlen(e.what())).c_str(), L"Unrecoverable general error.", MB_OK | MB_ICONERROR);
        PostQuitMessage(0);
    }
    URM::Core::Logger::DisposeLogger();

    return returnCode;
}