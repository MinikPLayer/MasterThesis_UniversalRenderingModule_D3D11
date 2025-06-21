#include "pch.h"
#include "Engine.h"
#include "SceneMesh.h"

namespace URM::Engine {
	// TODO: Move these structs to another file.
	struct VertexConstantBuffer {
		// ReSharper disable once CppInconsistentNaming
		Matrix WVP;
		Matrix worldMatrix;
		Matrix inverseWorldMatrix;
	};

	// Alignment rules: https://maraneshi.github.io/HLSL-ConstantBufferLayoutVisualizer/
	struct PixelConstantBuffer {
		static constexpr int MAX_LIGHTS_COUNT = 8;
		
		struct Material {
			alignas(4) int useAlbedoTexture = 0;
		};

		struct alignas(16) Light {
			alignas(16) Vector3 color;
			alignas(16) Vector3 position;
			alignas(4) float ambientIntensity;
			alignas(4) float diffuseIntensity;
			alignas(4) float specularIntensity;

			// ReSharper disable once CppInconsistentNaming
			int _padding_;

			// ReSharper disable once CppPossiblyUninitializedMember
			Light(Vector3 position = Vector3::Zero,
			      Color color = Color(1, 1, 1),
			      float ambient = 0.05f,
			      float diffuse = 0.9f,
			      float specular = 1.0f) : color(color.ToVector3()), position(position), ambientIntensity(ambient), diffuseIntensity(diffuse), specularIntensity(specular) {}
		};

		alignas(4) Vector4 viewPosition;
		alignas(16) Material material;
		alignas(4) uint32_t activeLightsCount = 0;
		alignas(16) Light lights[8];


		PixelConstantBuffer(Vector3 viewPos) : viewPosition(viewPos.x, viewPos.y, viewPos.z, 1.0f) {}
	};

	struct WVPMatrix {
		Matrix wvp;
		Matrix world;

		void Apply(VertexConstantBuffer& buffer) const {
			buffer.WVP = XMMatrixTranspose(wvp);
			buffer.worldMatrix = XMMatrixTranspose(world);
			buffer.inverseWorldMatrix = XMMatrixInverse(nullptr, world);
		}

		WVPMatrix(Matrix projection, Matrix view, Matrix world) : world(world) {
			wvp = world * view * projection;
		}
	};

	namespace {
		WVPMatrix CreateTransformationMatrix(CameraObject* camera, Core::Size2i windowSize) {
			auto proj = camera->CalculateProjectionMatrix(windowSize);
			auto view = camera->CalculateViewMatrix();
			auto world = Matrix::Identity;

			return {proj, view, world};
		}
	}
	// ============================================================

	void Engine::Update() {
		mCore.GetWindow().PollEvents();

		mTimer.Update();
		if (onUpdate) {
			this->onUpdate(*this);
		}
	}

	void Engine::Present(int verticalSyncInterval) {
		mCore.Present(verticalSyncInterval);
	}

	bool Engine::ShouldClose() const {
		return this->mCore.GetWindow().IsDestroyed();
	}

	void Engine::Clear(Color color) {
		mCore.Clear(color);
	}

	void Engine::Clear() {
		this->Clear(renderParameters.clearColor);
	}

	void Engine::Draw(RenderingParams& params, std::weak_ptr<CameraObject> mainCamera, std::vector<std::weak_ptr<SceneMesh>>& meshes, std::vector<std::weak_ptr<Light>>& lights) {
		if (mainCamera.expired()) {
			throw std::runtime_error("Main camera is not set. Cannot draw the scene.");
		}
		
		auto cameraPtr = mainCamera.lock();
		auto context = this->mCore.GetContext();

		auto vp = params.viewport.GetData();
		auto windowSize = this->mCore.GetWindow().GetSize();
		vp.size = windowSize;
		params.viewport.SetData(vp);
		params.viewport.Bind(this->mCore);

		params.albedoTextureSampler.Bind(mCore, 0);

		this->mCore.SetPrimitiveTopology(params.topology);
		params.rasterizerState.Bind(this->mCore);

		this->mVertexConstantBuffer.Bind(this->mCore, 0);
		this->mPixelConstantBuffer.Bind(this->mCore, 1);

		auto cameraPos = cameraPtr->GetTransform().GetPosition();
		// TODO: Add support for custom PixelConstantBuffer types.
		auto pixelBufferValue = PixelConstantBuffer(cameraPos);
		size_t lightsCount = lights.size();
		if (lightsCount > PixelConstantBuffer::MAX_LIGHTS_COUNT) {
			spdlog::warn("Too many lights in the scene. Max supported: {}, current: {}. Truncating to the max value.", PixelConstantBuffer::MAX_LIGHTS_COUNT, lightsCount);
			lightsCount = PixelConstantBuffer::MAX_LIGHTS_COUNT;
		}
		pixelBufferValue.activeLightsCount = static_cast<uint32_t>(lightsCount);
		for (size_t i = 0; i < lightsCount; i++) {
			auto l = lights[i].lock();
			pixelBufferValue.lights[i] = PixelConstantBuffer::Light(
				l->GetTransform().GetPosition(),
				l->color,
				l->ambientIntensity,
				l->diffuseIntensity,
				l->specularIntensity
			);
		}

		this->mPixelConstantBuffer.UpdateWithData(this->mCore, &pixelBufferValue);
		
		auto renderMatrix = CreateTransformationMatrix(cameraPtr.get(), windowSize);

		// TODO: Group meshes by shaders and input layouts.
		for (auto& mesh : meshes) {
			auto sceneMesh = mesh.lock();
			
			VertexConstantBuffer vcb;
			auto worldMatrix = sceneMesh->GetTransform().GetWorldSpaceMatrix();
			auto meshRenderMatrix = renderMatrix;
			meshRenderMatrix.world = worldMatrix;
			meshRenderMatrix.wvp = worldMatrix * renderMatrix.wvp;
			meshRenderMatrix.Apply(vcb);
			this->mVertexConstantBuffer.UpdateWithData(this->mCore, &vcb);

			sceneMesh->GetInputLayout()->Bind(mCore);
			sceneMesh->GetShader()->Bind(mCore);

			auto& m = sceneMesh->GetMesh();

			// TODO: Combine similiar meshes to avoid multiple data sending.
			// TODO: Add support for materials.
			if (m.ContainsTextures()) {
				pixelBufferValue.material.useAlbedoTexture = 1;
				m.BindTextures(mCore);
			}
			else {
				pixelBufferValue.material.useAlbedoTexture = 0;
			}
			this->mPixelConstantBuffer.UpdateWithData(this->mCore, &pixelBufferValue);

			m.GetVertexBuffer().Bind(this->mCore, 0);
			if (m.ContainsIndices()) {
				m.GetIndexBuffer().Bind(mCore, 0);

				context->DrawIndexed(m.GetIndicesCount(), 0, 0);
			}
			else {
				m.GetVertexBuffer().Bind(this->mCore, 0);
				context->Draw(m.GetVerticesCount(), 0);
			}
		}
	}

	void Engine::Draw(RenderingParams& params, Scene& scene) {
		this->Draw(params, mScene.GetMainCamera(), mScene.GetMeshes(), scene.GetLights());
	}
	
	void Engine::Draw(RenderingParams& params) {
		this->Draw(params, mScene);
	}

	void Engine::RunLoop() {
		while (!mCore.GetWindow().IsDestroyed()) {
			this->Update();

			this->Clear(renderParameters.clearColor);
			this->Draw(renderParameters);
			this->Present(vSyncInterval);
		}
	}

	Engine::Engine(const Core::WindowCreationParams& windowParams) : mCore(windowParams),
	                                                                 mScene(mCore),
	                                                                 mVertexConstantBuffer(Core::D3DConstantBuffer::Create<VertexConstantBuffer>(mCore, Core::ShaderStages::VERTEX)),
	                                                                 mPixelConstantBuffer(Core::D3DConstantBuffer::Create<PixelConstantBuffer>(mCore, Core::ShaderStages::PIXEL)) {}

}
