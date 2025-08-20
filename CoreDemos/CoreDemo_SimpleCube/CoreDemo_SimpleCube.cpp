#include <URM/Core/D3DCore.h>
#include <URM/Core/ModelLoader.h>
#include <URM/Core/D3DConstantBuffer.h>
#include <URM/Core/D3DViewport.h>
#include <URM/Core/D3DRasterizerState.h>
#include <chrono>
#include <directxtk/SimpleMath.h>
#include <URM/Core/D3DInputLayout.h>

using namespace DirectX;

const DirectX::SimpleMath::Vector2 DirectX::SimpleMath::Vector2::Zero = DirectX::SimpleMath::Vector2(0.0f, 0.0f);

struct VertexConstantBufferData {
	Matrix worldViewPerspective;
	Matrix worldMatrix;
	Matrix inverseWorldMatrix;
};

struct WVPMatrix {
	Matrix worldViewPerspective;
	Matrix world;

	void Apply(VertexConstantBufferData& buffer) const {
		buffer.worldViewPerspective = XMMatrixTranspose(worldViewPerspective);
		buffer.worldMatrix = XMMatrixTranspose(world);
		buffer.inverseWorldMatrix = XMMatrixInverse(nullptr, world);
	}

	WVPMatrix(Matrix projection, Matrix view, Matrix world) : world(world) {
		worldViewPerspective = world * view * projection;
	}
};

WVPMatrix CreateTransformationMatrix(
	const Vector3& modelPosition,
	const Vector3& modelRotationAngles, // Degrees
	const Vector3& modelScale,
	const Vector3& cameraPosition,
	const Vector3& cameraTarget,
	const Vector3& cameraUpDirection,
	float fov, // Degrees
	float nearPlane,
	float farPlane,
	URM::Core::Size2i windowSize
) {
	// 1. World Matrix
	Matrix matScale = XMMatrixScaling(modelScale.x, modelScale.y, modelScale.z);
	Matrix matRotation = XMMatrixRotationRollPitchYaw(
		XMConvertToRadians(modelRotationAngles.x),
		XMConvertToRadians(modelRotationAngles.y),
		XMConvertToRadians(modelRotationAngles.z)
	);
	Matrix matTranslation = XMMatrixTranslation(modelPosition.x, modelPosition.y, modelPosition.z);
	Matrix matWorld = matScale * matRotation * matTranslation;

	// 2. View Matrix
	auto vecCameraPosition = DirectX::XMLoadFloat3(&cameraPosition);
	auto vecCameraTarget = DirectX::XMLoadFloat3(&cameraTarget);
	auto vecCameraUp = DirectX::XMLoadFloat3(&cameraUpDirection);
	auto matView = DirectX::XMMatrixLookAtLH(vecCameraPosition, vecCameraTarget, vecCameraUp);

	// 3. Projection Matrix (Orthographic)
	Matrix matProjection = XMMatrixPerspectiveFovLH(
		XMConvertToRadians(fov),
		static_cast<float>(windowSize.width) / static_cast<float>(windowSize.height),
		nearPlane,
		farPlane
	);

	return { matProjection, matView, matWorld };
}

constexpr Vector3 CAM_POS = { 4.0f, 4.0f, 4.0f };
constexpr Vector3 CAM_TARGET = { 0.0f, 0.0f, 0.0f };
constexpr Vector3 CAM_UP = { 0.0f, 1.0f, 0.0f };
WVPMatrix TestDrawCreateWvp(Vector3 positionOffset, URM::Core::Size2i windowSize, float rotation, float scale = 1.0f) {
	Vector3 modelPos = positionOffset;
	Vector3 modelRot = { 0.0f, rotation, 0.0f };
	Vector3 modelScl = { scale, scale, scale };

	float fov = 45.f;
	float nearPlane = 1.0f;
	float farPlane = 100.0f;

	auto wvp = CreateTransformationMatrix(
		modelPos,
		modelRot,
		modelScl,
		CAM_POS,
		CAM_TARGET,
		CAM_UP,
		fov,
		nearPlane,
		farPlane,
		windowSize
	);

	return wvp;
}


void Draw(URM::Core::D3DCore& core, std::shared_ptr<URM::Core::ModelLoaderNode> node) {
    for (auto m : node->meshes) {
		m->GetVertexBuffer().Bind(core, 0);
		m->GetIndexBuffer().Bind(core, 0);

        core.GetContext()->DrawIndexed(m->GetIndicesCount(), 0, 0);
    }

    for (auto& child : node->children) {
        Draw(core, child);
	}
}

int WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow) {
    URM::Core::D3DCore core(URM::Core::WindowCreationParams(800, 600, "Core Demo - Cube", hInstance));
    auto texturesPool = std::map<std::string, URM::Core::D3DTexture2D>();

    auto rStateData = URM::Core::D3DRasterizerStateData();
    rStateData.cullMode = URM::Core::CullModes::NONE;
	auto rState = URM::Core::D3DRasterizerState(rStateData);
	rState.Bind(core);

	auto viewport = URM::Core::D3DViewport(URM::Core::D3DViewportData(core.GetWindow().GetSize()));
	viewport.Bind(core);

	core.SetPrimitiveTopology(URM::Core::PrimitiveTopologies::TRIANGLE_LIST);

	auto vertexShader = URM::Core::VertexShader(core, L"VerySimpleVertexShader.cso");
	vertexShader.Bind(core);

	auto pixelShader = URM::Core::PixelShader(core, L"VerySimplePixelShader.cso");
	pixelShader.Bind(core);

    auto inputLayout = URM::Core::ModelLoaderLayout(core, vertexShader);
	inputLayout.Bind(core);

	auto vertexConstantBuffer = URM::Core::D3DConstantBuffer::Create<VertexConstantBufferData>(core, URM::Core::ShaderStages::VERTEX);
	auto matrix = TestDrawCreateWvp(
		{ 0.0f, 0.0f, 0.0f },
		core.GetWindow().GetSize(),
		0.0f
	);
	auto vertexData = VertexConstantBufferData();
	matrix.Apply(vertexData);
	vertexConstantBuffer.Bind(core, 0);
	vertexConstantBuffer.UpdateWithData(core, &vertexData);

    auto node = URM::Core::ModelLoader::LoadFromFile(
        core,
        texturesPool,
        "cube.glb"
    );

    while (!core.GetWindow().IsDestroyed()) {
        core.GetWindow().PollEvents();
        core.ClearFramebuffer(DirectX::SimpleMath::Color(0.3, 0.5, 1.0));

        Draw(core, node);

        core.Present(0);
    }
}