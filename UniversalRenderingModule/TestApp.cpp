// ReSharper disable CppClangTidyCppcoreguidelinesAvoidConstOrRefDataMembers
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
#include <URM/Engine/Scene.h>

#include <URM/Engine/ModelObject.h>
#include <URM/Engine/MeshObject.h>

#include <URM/Engine/Engine.h>

// #include <DirectXMath.h>

using namespace DirectX;

// Notes:
// - Ograniczenia: 
//      - Jeden silnik = jedno okno. 

#ifdef __clang__
#pragma clang diagnostic ignored "-Wcovered-switch-default"
#pragma clang diagnostic ignored "-Wswitch-enum"
#endif

// Indicates to hybrid graphics systems to prefer the discrete part by default
extern "C" {
	__declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
	__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}

struct VertexConstantBuffer {
	Matrix worldViewPerspective;
	Matrix worldMatrix;
	Matrix inverseWorldMatrix;
};

// struct PixelConstantBuffer {
// 	Vector4 viewPosition;
//
// 	struct Light {
// 		Vector4 color;
// 		Vector4 position;
// 		float ambientIntensity;
// 		float diffuseIntensity;
// 		float specularIntensity;
//
// 		// ReSharper disable once CppInconsistentNaming
// 		int _padding_;
//
// 		// ReSharper disable once CppPossiblyUninitializedMember
// 		Light(float r,
// 		      float g,
// 		      float b,
// 		      float x,
// 		      float y,
// 		      float z,
// 		      float ambient,
// 		      float diffuse,
// 		      float specular) : color(r, g, b, 1.0f), position(x, y, z, 1.0f), ambientIntensity(ambient), diffuseIntensity(diffuse), specularIntensity(specular) {}
// 	} light;
//
// 	struct Material {
// 		int useAlbedoTexture = 0;
// 		// ReSharper disable once CppInconsistentNaming
// 		Vector3 _padding_;
// 	} material;
//
// 	PixelConstantBuffer(const Light& light, Vector3 viewPos) : viewPosition(viewPos.x, viewPos.y, viewPos.z, 1.0f), light(light) {}
// };

// ReSharper disable once CppInconsistentNaming
struct WVPMatrix {
	Matrix worldViewPerspective;
	Matrix world;

	void Apply(VertexConstantBuffer& buffer) const {
		buffer.worldViewPerspective = XMMatrixTranspose(worldViewPerspective);
		buffer.worldMatrix = XMMatrixTranspose(world);
		buffer.inverseWorldMatrix = XMMatrixInverse(nullptr, world);
	}

	WVPMatrix(Matrix projection, Matrix view, Matrix world) : world(world) {
		worldViewPerspective = world * view * projection;
	}
};

namespace {
	std::chrono::high_resolution_clock::time_point LastTime = std::chrono::high_resolution_clock::now();
	std::chrono::high_resolution_clock::time_point ProgramStartTime = std::chrono::high_resolution_clock::now();
	void Clear(URM::Core::D3DCore& core) {
		auto now = std::chrono::high_resolution_clock::now();
		auto deltaTime = std::chrono::duration_cast<std::chrono::microseconds>(now - LastTime).count() / 1000000.0f;
		UNREFERENCED_PARAMETER(deltaTime);
		LastTime = now;

		core.ClearFramebuffer(Color(Colors::Black));
	}

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
		auto matView = DirectX::XMMatrixLookAtRH(vecCameraPosition, vecCameraTarget, vecCameraUp);

		// 3. Projection Matrix (Orthographic)
		Matrix matProjection = XMMatrixPerspectiveFovRH(
			XMConvertToRadians(fov),
			static_cast<float>(windowSize.width) / static_cast<float>(windowSize.height),
			nearPlane,
			farPlane
		);

		return {matProjection, matView, matWorld};
	}


	struct TestDrawData {
		URM::Core::D3DCore& core;
		URM::Core::D3DConstantBuffer& vertexConstantBuffer;
		URM::Core::D3DConstantBuffer& pixelConstantBuffer;
		URM::Core::D3DViewport& viewport;
		URM::Core::D3DRasterizerState& rState;
		URM::Core::D3DSampler& sampler;
		URM::Engine::Scene& scene;
	};

	void TestDrawMesh(TestDrawData& data, URM::Engine::PixelConstantBuffer pcb, std::weak_ptr<URM::Engine::MeshObject> mesh, WVPMatrix transformMatrix) {
		auto nodeWorldMatrix = mesh.lock()->GetTransform().GetWorldSpaceMatrix();
		VertexConstantBuffer cBufferData;
		transformMatrix.world = nodeWorldMatrix * transformMatrix.world;
		transformMatrix.worldViewPerspective = nodeWorldMatrix * transformMatrix.worldViewPerspective;
		transformMatrix.Apply(cBufferData);
		data.vertexConstantBuffer.UpdateWithData(data.core, &cBufferData);

		auto sceneMesh = mesh.lock();
		sceneMesh->GetInputLayout()->Bind(data.core);
		sceneMesh->GetShader()->Bind(data.core);

		auto& m = mesh.lock()->GetMesh();
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

	constexpr Vector3 CAM_POS = {0.0f, 4.0f, -8.0f};
	constexpr Vector3 CAM_TARGET = {0.0f, 0.0f, 0.0f};
	constexpr Vector3 CAM_UP = {0.0f, 1.0f, 0.0f};
	WVPMatrix TestDrawCreateWvp(Vector3 positionOffset, URM::Core::Size2i windowSize, float rotation, float scale = 1.0f) {
		Vector3 modelPos = positionOffset;
		Vector3 modelRot = {0.0f, rotation, 0.0f};
		Vector3 modelScl = {scale, scale, scale};

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

	class ITest {  // NOLINT(cppcoreguidelines-special-member-functions)
	public:
		virtual ~ITest() = default;
		virtual void Init(URM::Core::D3DCore& core, URM::Engine::Scene& scene);
		virtual void Update(URM::Engine::Engine& engine);
	};

	void ITest::Init(URM::Core::D3DCore& core, URM::Engine::Scene& scene) {
		throw std::logic_error("Not implemented");
	}

	void ITest::Update(URM::Engine::Engine& engine) {
		throw std::logic_error("Not implemented");
	}

	// ReSharper disable once CppClassNeverUsed
	class SceneRelativeTransformationsTest : public ITest {
	public:
		void Init(URM::Core::D3DCore& core, URM::Engine::Scene& scene) override {
			auto cubeModel = new URM::Engine::ModelObject("cube_textured.glb");
			auto parentCube = scene.GetRoot().lock()->AddChild(cubeModel);
			parentCube->GetTransform().SetLocalPosition({0.0f, 0.0f, 0.0f});
			
			auto cube = parentCube->AddChild(new URM::Engine::ModelObject("cube_textured.glb"));
			cube->GetTransform().SetLocalPosition({3.0f, 0.0f, 0.0f});
			cube->GetTransform().SetLocalScale({0.5f, 0.5f, 0.5f});

			cube = cube->AddChild(new URM::Engine::ModelObject("cube_textured.glb"));
			cube->GetTransform().SetLocalPosition({3.0f, 0.0f, 0.0f});
			cube->GetTransform().SetLocalScale({0.5f, 0.5f, 0.5f});

			cube = cube->AddChild(new URM::Engine::ModelObject("cube_textured.glb"));
			cube->GetTransform().SetLocalPosition({3.0f, 0.0f, 0.0f});
			cube->GetTransform().SetLocalScale({0.5f, 0.5f, 0.5f});

			auto staticPosObject = cube->AddChild(new URM::Engine::ModelObject("suzanne.glb"));
			staticPosObject->GetTransform().SetLocalPosition({6.0f, 0.0f, 0.0f});
			staticPosObject->GetTransform().SetLocalScale({3.0f, 3.0f, 3.0f});

			auto light = parentCube->AddChild(
				new URM::Engine::LightObject()
			);
			light->ambientIntensity = 0.2f;
			light->GetTransform().SetLocalPosition(Vector3(0, 3, 0));
			
			light = cube->AddChild(
				new URM::Engine::LightObject()
			);
			light->color = Color(1, 0, 0);
			
			auto camera = scene.GetRoot().lock()->AddChild(new URM::Engine::CameraObject(60.0f));
			camera->GetTransform().SetPosition(Vector3(8.0f, 8.0f, -8.0f));
			camera->GetTransform().LookAt(Vector3(0.0f, 0.0f, 0.0f));
			scene.SetMainCamera(camera);
		}

		void Update(URM::Engine::Engine& engine) override {
			auto root = engine.GetScene().GetRoot();
			auto cubeObject = root.lock()->GetChildByIndex(0);
			auto smallCubeObject = cubeObject->GetChildByIndex(1);
			auto superSmallCube = smallCubeObject->GetChildByIndex(1);
			auto staticPosObject = superSmallCube->GetChildByIndex(1)->GetChildByIndex(1);

			cubeObject->GetTransform().SetLocalRotation(Vector3(0, engine.GetTimer().GetElapsedTime() * 90.0f, 0));
			smallCubeObject->GetTransform().SetLocalRotation(Vector3(0, 0, engine.GetTimer().GetElapsedTime() * 1.7f * 90.0f));
			superSmallCube->GetTransform().SetLocalRotation(Vector3(0, engine.GetTimer().GetElapsedTime() * 2.3f * 90.0f, 0));
			
			staticPosObject->GetTransform().SetPosition(Vector3(2, 2, 2));

			const auto camera = engine.GetScene().GetMainCamera();
			const auto& timer = engine.GetTimer();
			auto& cameraTransform = camera.lock()->GetTransform();
			const auto scaledTime = timer.GetElapsedTime() * 2.0f;
			
			auto distance = 10.0f;
			cameraTransform.SetLocalPosition({distance * sin(scaledTime), distance * sin(scaledTime * 0.7f), distance * cos(scaledTime)});
			cameraTransform.LookAt(Vector3(0, 0, 0));
			
			staticPosObject->GetTransform().LookAt(cameraTransform.GetPosition());
		}
	};

	class MultipleShadersTest : public ITest {
		std::vector<std::weak_ptr<URM::Engine::LightObject>> mLights;
		
	public:
		void Init(URM::Core::D3DCore& core, URM::Engine::Scene& scene) override {
			auto alternativeShader = std::make_shared<URM::Core::ShaderProgram>(URM::Core::ShaderProgram(core, L"SimpleVertexShader.cso", L"ColorOnlyPixelShader.cso"));
			auto alternativeLayout = std::make_shared<URM::Core::ModelLoaderLayout>(URM::Core::ModelLoaderLayout(core, *alternativeShader));

			auto suzanneModel = new URM::Engine::ModelObject(
				"suzanne.glb",
				alternativeShader,
				alternativeLayout
			);
			auto suzanne = scene.GetRoot().lock()->AddChild(suzanneModel);
			suzanne->GetTransform().SetLocalPosition({-2.0f, 0.0f, 0.0f});
			suzanne->GetTransform().SetLocalRotation({0.0f, 180.0f, 0.0f});

			auto cube = scene.GetRoot().lock()->AddChild(new URM::Engine::ModelObject("cube_textured.glb"));
			cube->GetTransform().SetLocalPosition({2.0f, 0.0f, 0.0f});

			auto floorCube = scene.GetRoot().lock()->AddChild(
				new URM::Engine::ModelObject(
					"cube.glb",
					alternativeShader,
					alternativeLayout
				)
			);
			floorCube->GetTransform().SetLocalPosition({0.0f, -2.0f, 0.0f});
			floorCube->GetTransform().SetLocalScale({5.0f, 1.0f, 5.0f});

			auto wallCube = scene.GetRoot().lock()->AddChild(
				new URM::Engine::ModelObject(
					"cube.glb",
					alternativeShader,
					alternativeLayout
				)
			);
			wallCube->GetTransform().SetLocalPosition({0.0f, 3.0f, 5.0f});
			wallCube->GetTransform().SetLocalScale({5.0f, 5.0f, 1.0f});

			auto leftWallCube = scene.GetRoot().lock()->AddChild(
				new URM::Engine::ModelObject(
					"cube.glb",
					alternativeShader,
					alternativeLayout
				)
			);
			leftWallCube->GetTransform().SetLocalPosition({-5.0f, 3.0f, 0.0f});
			leftWallCube->GetTransform().SetLocalScale({1.0f, 5.0f, 5.0f});

			auto rightWallCube = scene.GetRoot().lock()->AddChild(
				new URM::Engine::ModelObject(
					"cube.glb",
					alternativeShader,
					alternativeLayout
				)
			);
			rightWallCube->GetTransform().SetLocalPosition({5.0f, 3.0f, 0.0f});
			rightWallCube->GetTransform().SetLocalScale({1.0f, 5.0f, 5.0f});

			// Lights
			auto newLight = scene.GetRoot().lock()->AddChild(
				std::make_shared<URM::Engine::LightObject>()
			);
			newLight->color = Color(1, 0, 0);
			mLights.push_back(newLight);

			newLight = scene.GetRoot().lock()->AddChild(
				std::make_shared<URM::Engine::LightObject>()
			);
			newLight->color = Color(0, 1, 0);
			mLights.push_back(newLight);

			newLight = scene.GetRoot().lock()->AddChild(
				std::make_shared<URM::Engine::LightObject>()
			);
			newLight->color = Color(0, 0, 1);
			mLights.push_back(newLight);

			// Camera
			const auto camera = scene.GetRoot().lock()->AddChild(new URM::Engine::CameraObject());
			camera->GetTransform().SetPosition(Vector3(0.0f, 3.0f, -6.0f));
			camera->GetTransform().LookAt(Vector3(0.0f, 0.0f, 0.0f));
			scene.SetMainCamera(camera);
		}

		void Update(URM::Engine::Engine& engine) override {
			constexpr float lightDistance = 2.1f;
			
			auto rotation = engine.GetTimer().GetElapsedTime() * 90.0f;
			auto rotationRad = rotation * DirectX::XM_PI / 180.0f;

			mLights[0].lock()->GetTransform().SetPosition(Vector3(sin(rotationRad) * lightDistance, lightDistance / 1.5f, cos(rotationRad) * lightDistance));
			mLights[1].lock()->GetTransform().SetPosition(Vector3(sin(rotationRad + 2.1f) * lightDistance, lightDistance / 1.5f, cos(rotationRad + 2.1f) * lightDistance));
			mLights[2].lock()->GetTransform().SetPosition(Vector3(sin(rotationRad + 4.2f) * lightDistance, lightDistance / 1.5f, cos(rotationRad + 4.2f) * lightDistance));

			// auto camera = engine.GetScene().GetMainCamera();
			// camera.lock()->GetTransform().SetRotation(
			// 	Quaternion::Rot
			// );
		}
	};

	constexpr bool ENGINE_MODE = true;
	constexpr bool ENGINE_LOOP_MODE = true;
	constexpr bool ENGINE_TRACE_MODE = true;
	auto SelectedTest = std::unique_ptr<ITest>(std::make_unique<SceneRelativeTransformationsTest>());

	void Init(URM::Core::D3DCore& core, URM::Engine::Scene& scene) {
		SelectedTest->Init(core, scene);
	}

	void Update(URM::Engine::Engine& engine) {
		SelectedTest->Update(engine);
	}

	template<URM::Core::VertexTypeConcept V>
	void TestDraw(TestDrawData data) {
		// Aktualizacja stałej buforowej
		auto elapsedTime = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - ProgramStartTime).count() / 1000000.0f;

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

		constexpr float lightDistance = 2.1f;
		auto lightPosition = Vector3(
			sin(rotationRad) * lightDistance,
			lightDistance / 1.5f,
			cos(rotationRad) * lightDistance
		);
		auto pixelBufferValue = URM::Engine::PixelConstantBuffer(
			CAM_POS
		);
		pixelBufferValue.activeLightsCount = 1;
		pixelBufferValue.lights[0] = URM::Engine::PixelConstantBuffer::Light();
		pixelBufferValue.lights[0].position = lightPosition;
		data.pixelConstantBuffer.UpdateWithData(data.core, &pixelBufferValue);

		auto wvp = TestDrawCreateWvp({0.0f, 0.0f, 0.0f}, windowSize, 0);
		for (auto& mesh : data.scene.GetMeshes()) {
			TestDrawMesh(data, pixelBufferValue, mesh, wvp);
		}

		data.core.Present(0);
	}

	int ActualMainEngine(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow) {
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
		Init(engine.GetCore(), scene);
		engine.onUpdate = Update;

		if (ENGINE_LOOP_MODE) {
			ENGINE_TRACE_MODE ? engine.RunLoopTrace() : engine.RunLoop();
		}
		else {
			float deltaCounter = 0;
			constexpr float drawInterval = 1.0;
			while (!engine.ShouldClose()) {
				engine.Update();

				deltaCounter += engine.GetTimer().GetDeltaTime();
				if (deltaCounter > drawInterval) {
					engine.Clear();
					engine.Draw(engine.renderParameters, scene.GetMainCamera(), scene.GetMeshes(), scene.GetLights());
					engine.Present(0);

					deltaCounter -= drawInterval;
				}
			}
		}


		return 0;
	}

	int ActualMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow) {
		UNREFERENCED_PARAMETER(hPrevInstance);
		UNREFERENCED_PARAMETER(lpCmdLine);
		UNREFERENCED_PARAMETER(nCmdShow);

		if (!XMVerifyCPUSupport())
			return 1;

		HRESULT hr = CoInitializeEx(nullptr, COINITBASE_MULTITHREADED);
		if (FAILED(hr))
			return 1;

		URM::Core::D3DCore core(URM::Core::WindowCreationParams(1600, 1000, "UniversalRenderingModule", hInstance));
		URM::Engine::Scene scene(core);

		auto suzanneModel = new URM::Engine::ModelObject("suzanne.glb");
		auto suzanne = scene.GetRoot().lock()->AddChild(suzanneModel);
		suzanne->GetTransform().SetLocalPosition({-2.0f, 0.0f, 0.0f});

		auto cubeModel = new URM::Engine::ModelObject("cube_textured.glb");
		auto cube = scene.GetRoot().lock()->AddChild(cubeModel);
		cube->GetTransform().SetLocalPosition({2.0f, 0.0f, 0.0f});

		URM::Core::D3DConstantBuffer vertexConstantBuffer = URM::Core::D3DConstantBuffer::Create<VertexConstantBuffer>(core, URM::Core::ShaderStages::VERTEX);
		URM::Core::D3DConstantBuffer pixelConstantBuffer = URM::Core::D3DConstantBuffer::Create<URM::Engine::PixelConstantBuffer>(core, URM::Core::ShaderStages::PIXEL);
		URM::Core::D3DViewport viewport(URM::Core::D3DViewportData(core.GetWindow().GetSize()));

		auto rStateData = URM::Core::D3DRasterizerStateData();
		rStateData.cullMode = URM::Core::CullModes::BACK;
		auto rState = URM::Core::D3DRasterizerState(rStateData);

		auto sampler = URM::Core::D3DSampler(URM::Core::D3DSamplerData());

		auto testDrawData = TestDrawData{
			.core = core,
			.vertexConstantBuffer = vertexConstantBuffer,
			.pixelConstantBuffer = pixelConstantBuffer,
			.viewport = viewport,
			.rState = rState,
			.sampler = sampler,
			.scene = scene
		};

		core.onWindowPaint = [&](URM::Core::D3DCore& c)
		{
			Clear(c);
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

	std::string GetLastErrorAsString() {
		//Get the error message ID, if any.
		DWORD errorMessageId = GetLastError();
		if (errorMessageId == 0) {
			return {}; //No error message has been recorded
		}

		LPSTR messageBuffer = nullptr;

		//Ask Win32 to give us the string version of that message ID.
		//The parameters we pass in, tell Win32 to create the buffer that holds the message for us (because we don't yet know how long the message string will be).
		size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		                             nullptr,
		                             errorMessageId,
		                             MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		                             reinterpret_cast<LPSTR>(&messageBuffer),
		                             0,
		                             nullptr);

		//Copy the error message into a std::string.
		std::string message(messageBuffer, size);

		//Free the Win32's string's buffer.
		LocalFree(messageBuffer);

		return message;
	}
}


// Entry point
int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow) {
	URM::Core::Logger::InitLogger();
	int returnCode = 123456;

#ifdef NDEBUG
	try {
		returnCode = ENGINE_MODE ? ActualMainEngine(hInstance, hPrevInstance, lpCmdLine, nCmdShow) : ActualMain(hInstance, hPrevInstance, lpCmdLine, nCmdShow);
	}
	catch (std::exception& e) {
		spdlog::critical("Exception: {}", e.what());

		std::wstring msgBoxMessage = L"Unrecoverable error: \n" + std::wstring(e.what(), e.what() + strlen(e.what()));

		if (GetLastError() != 0) {
			auto lastWinApiErrorString = GetLastErrorAsString();
			msgBoxMessage += L"\n\nLast WinAPI error: " + std::wstring(lastWinApiErrorString.begin(), lastWinApiErrorString.end());
		}

		MessageBox(nullptr, std::wstring(e.what(), e.what() + strlen(e.what())).c_str(), L"Unrecoverable general error.", MB_OK | MB_ICONERROR);
		PostQuitMessage(0);
	}
#else
	returnCode = ENGINE_MODE ? ActualMainEngine(hInstance, hPrevInstance, lpCmdLine, nCmdShow) : ActualMain(hInstance, hPrevInstance, lpCmdLine, nCmdShow);
#endif
	URM::Core::Logger::DisposeLogger();

	return returnCode;
}
