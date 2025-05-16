#include <spdlog/spdlog.h>

#include <URM/Core/pch.h>
#include <URM/Core/Window.h>
#include <URM/Core/Log.h>
#include <URM/Core/D3DCore.h>
#include <URM/Core/Mesh.h>
#include <URM/Core/VertexPosition.h>
#include <URM/Core/ID3DBuffer.h>
#include <URM/Core/ShaderProgram.h>
#include <URM/Core/D3DInputLayout.h>

#include <URM/Core/D3DViewport.h>
#include <URM/Core/D3DConstantBuffer.h>
#include <URM/Core/D3DRasterizerState.h>

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
void Clear(D3DCore& core) {
    auto now = std::chrono::high_resolution_clock::now();
    auto deltaTime = std::chrono::duration_cast<std::chrono::microseconds>(now - lastTime).count() / 1000000.0f;
    UNREFERENCED_PARAMETER(deltaTime);
    lastTime = now;

    auto elapsedTime = std::chrono::duration_cast<std::chrono::microseconds>(now - programStartTime).count() / 1000000.0f;

    core.Clear(DirectX::XMVECTORF32{ sin(elapsedTime / 2.0f) / 2.0f + 0.5f, cos(elapsedTime / 3.f) / 2.0f + 0.5f, 0.0f, 1.0f });
}

#pragma region ConstantBufferTest

struct ConstantBuffer
{
    DirectX::XMMATRIX mWorldViewProjection;
};


DirectX::XMMATRIX CreateTransformationMatrix(
    const DirectX::XMFLOAT3& modelPosition,
    const DirectX::XMFLOAT3& modelRotationAngles, // Degrees
    const DirectX::XMFLOAT3& modelScale,
    const DirectX::XMFLOAT3& cameraPosition,
    const DirectX::XMFLOAT3& cameraTarget,
    const DirectX::XMFLOAT3& cameraUpDirection,
    float fov, // Degrees
    float nearPlane,
    float farPlane,
    Size2i windowSize
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
    DirectX::XMMATRIX matWorld = matTranslation * matRotation * matScale;

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

    // 4. Combine: World * View * Projection
    DirectX::XMMATRIX matWVP = matWorld * matView * matProjection;

    return matWVP;
}

struct TestDrawData {
    D3DCore& core;
    D3DConstantBuffer& constantBuffer;
    D3DViewport& viewport;
    D3DRasterizerState& rState;
    ShaderProgram& program;
    D3DInputLayout<VertexPositionColor>& iLayout;
    Mesh<VertexPositionColor>& mesh;

    TestDrawData(D3DCore& core, D3DConstantBuffer& constantBuffer, D3DViewport& viewport, D3DRasterizerState& rState, ShaderProgram& program, D3DInputLayout<VertexPositionColor>& iLayout, Mesh<VertexPositionColor>& mesh)
        : core(core), constantBuffer(constantBuffer), viewport(viewport), rState(rState), program(program), iLayout(iLayout), mesh(mesh) {
    }
};

static int cbCounter = 0;
template<VertexTypeConcept V>
void TestDraw(TestDrawData data) {
    auto context = data.core.GetContext();

    // Aktualizacja stałej buforowej
    DirectX::XMFLOAT3 modelPos = { 0.0f, 0.0f, 0.0f };
    DirectX::XMFLOAT3 modelRot = { 0.0f, cbCounter / 100.0f, 0.0f };
    DirectX::XMFLOAT3 modelScl = { 1.0f, 1.0f, 1.0f };
    DirectX::XMFLOAT3 camPos = { 2.0f, 2.0f, -2.0f };
    DirectX::XMFLOAT3 camTarget = { 0.0f, 0.0f, 0.0f };
    DirectX::XMFLOAT3 camUp = { 0.0f, 1.0f, 0.0f };

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
        data.core.GetWindow().GetSize()
    );

    ConstantBuffer cb{};
    cb.mWorldViewProjection = DirectX::XMMatrixTranspose(WVP);
    data.constantBuffer.UpdateWithData(data.core, &cb);
    cbCounter++;

    auto vp = data.viewport.GetData();
    vp.size = data.core.GetWindow().GetSize();
    data.viewport.SetData(vp);
    data.viewport.Bind(data.core);

    data.rState.Bind(data.core);

    data.core.SetPrimitiveTopology(PrimitiveTopologies::TRIANGLE_STRIP);
    data.iLayout.Bind(data.core);

    data.mesh.GetVertexBuffer().Bind(data.core);
    data.constantBuffer.Bind(data.core);

    data.program.Bind(data.core);

    context->Draw(3, 0);

    data.core.Present(0);
}

#pragma endregion

int actualMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow) {
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
    UNREFERENCED_PARAMETER(nCmdShow);

    if (!XMVerifyCPUSupport())
        return 1;

    HRESULT hr = CoInitializeEx(nullptr, COINITBASE_MULTITHREADED);
    if (FAILED(hr))
        return 1;

    D3DCore core(WindowCreationParams(1600, 1000, "UniversalRenderingModule", hInstance));

    auto vertices = {
            VertexPositionColor(0.0f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f),
            VertexPositionColor(0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f),
            VertexPositionColor(-0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f)
    };
    Mesh<VertexPositionColor> mesh(core, vertices);
    ShaderProgram shader(core, L"SimpleVertexShader.cso", L"SimplePixelShader.cso");

    D3DConstantBuffer constantBuffer = D3DConstantBuffer::Create<ConstantBuffer>(core, D3D11_BIND_CONSTANT_BUFFER);
    D3DInputLayout<VertexPositionColor> inputLayout(core, shader);
    D3DViewport viewport(D3DViewportData(core.GetWindow().GetSize()));

    auto rStateData = D3DRasterizerStateData();
    rStateData.cullMode = CullModes::BACK;
    auto rState = D3DRasterizerState(rStateData);

    auto testDrawData = TestDrawData(core, constantBuffer, viewport, rState, shader, inputLayout, mesh);

    core.OnWindowPaint = [&](D3DCore& core) {
        Clear(core);
        TestDraw<VertexPositionColor>(testDrawData);
        };

    while (!core.GetWindow().IsDestroyed()) {
        core.GetWindow().PollEvents();
        Clear(core);
        TestDraw<VertexPositionColor>(testDrawData);
    }
    Logger::DisposeLogger();

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
    Logger::InitLogger();
    int returnCode = 123456;
    try {
        returnCode = actualMain(hInstance, hPrevInstance, lpCmdLine, nCmdShow);
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
    Logger::DisposeLogger();

    return returnCode;
}