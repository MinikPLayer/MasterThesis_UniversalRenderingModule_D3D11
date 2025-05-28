#include "pch.h"
#include "Engine.h"
#include "SceneMesh.h"

#include <directxtk/SimpleMath.h>

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
		alignas(4) int activeLightsCount = 0;
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
		WVPMatrix CreateTransformationMatrix(
			Vector3 cameraPosition,
			Vector3 cameraTarget,
			Vector3 cameraUp,
			float fov, // Degrees
			float nearPlane,
			float farPlane,
			Core::Size2i windowSize
		) {
			// 2. View Matrix
			auto vecCameraPosition = DirectX::XMLoadFloat3(&cameraPosition);
			auto vecCameraTarget = DirectX::XMLoadFloat3(&cameraTarget);
			auto vecCameraUp = DirectX::XMLoadFloat3(&cameraUp);
			auto matView = DirectX::XMMatrixLookAtLH(vecCameraPosition, vecCameraTarget, vecCameraUp);
			//URM::Engine::SceneObject cameraObject;
			//cameraObject.GetTransform().SetLocalPosition(cameraPosition);
			//cameraObject.GetTransform().SetLocalRotation(Quaternion::Identity);
			//cameraObject.GetTransform().SetLocalScale(Vector3(1, 1, 1));
			//auto matView = cameraObject.GetTransform().GetWorldSpaceMatrix().Invert();

			// 3. Projection Matrix (Orthographic)
			Matrix matProjection = DirectX::XMMatrixPerspectiveFovLH(
				DirectX::XMConvertToRadians(fov),
				static_cast<float>(windowSize.width) / static_cast<float>(windowSize.height),
				nearPlane,
				farPlane
			);

			return {matProjection, matView, DirectX::XMMatrixIdentity()};
		}
	}

	// ============================================================

	void Engine::Update() {
		mCore.GetWindow().PollEvents();

		mTimer.Update();
		if (onUpdate)
			this->onUpdate(*this);
	}

	void Engine::Present(int verticalSyncInterval) {
		mCore.Present(verticalSyncInterval);
	}

	bool Engine::ShouldClose() {
		return this->mCore.GetWindow().IsDestroyed();
	}

	void Engine::Clear(Color color) {
		mCore.Clear(color);
	}

	void Engine::Clear() {
		this->Clear(renderParameters.clearColor);
	}

	void Engine::Draw(RenderingParams& params, std::vector<std::weak_ptr<SceneMesh>>& meshes) {
		auto context = this->mCore.GetContext();

		auto vp = params.viewport.GetData();
		auto windowSize = this->mCore.GetWindow().GetSize();
		vp.size = windowSize;
		params.viewport.SetData(vp);
		params.viewport.Bind(this->mCore);

		params.albedoTextureSampler.Bind(mCore, 0);

		this->mCore.SetPrimitiveTopology(params.toplogy);
		params.rasterizerState.Bind(this->mCore);

		this->mVertexConstantBuffer.Bind(this->mCore, 0);
		this->mPixelConstantBuffer.Bind(this->mCore, 1);

		// TODO: Add dynamic lights with separate type.
		auto rotation = this->mTimer.GetElapsedTime() * 90.0f;
		auto rotationRad = rotation * DirectX::XM_PI / 180.0f;
		auto cameraPos = Vector3(0.0f, 4.0f, -8.0f);
		constexpr float lightDistance = 2.1f;

		// TODO: Add support for custom PixelConstantBuffer types.
		auto pixelBufferValue = PixelConstantBuffer(cameraPos);
		pixelBufferValue.activeLightsCount = 3;
		pixelBufferValue.lights[0] = PixelConstantBuffer::Light(
			Vector3(sin(rotationRad) * lightDistance, lightDistance / 1.5f, cos(rotationRad) * lightDistance),
			Color(0, 0, 1),
			0.03f,
			0.9f,
			1.0f
		);
		pixelBufferValue.lights[1] = PixelConstantBuffer::Light(
			Vector3(sin(rotationRad + 2.1f) * lightDistance, lightDistance / 1.5f, cos(rotationRad + 2.1f) * lightDistance),
			Color(1, 0, 0),
			0.02f,
			0.9f,
			1.0f
		);
		pixelBufferValue.lights[2] = PixelConstantBuffer::Light(
			Vector3(sin(rotationRad + 4.2f) * lightDistance, lightDistance / 1.5f, cos(rotationRad + 4.2f) * lightDistance),
			Color(0, 1, 0),
			0.01f,
			0.9f,
			1.0f
		);

		this->mPixelConstantBuffer.UpdateWithData(this->mCore, &pixelBufferValue);

		// TODO: Add dynamic camera.
		WVPMatrix renderMatrix = CreateTransformationMatrix(
			cameraPos,
			Vector3(0.0f, 0.0f, 0.0f),
			Vector3::Up,
			45.0f,
			1.0f,
			100.0f,
			windowSize
		);

		// TODO: Group meshes by shaders and input layouts.
		for (auto& mesh : meshes) {
			auto sceneMesh = mesh.lock();
			if (sceneMesh == nullptr) {
				spdlog::error("What");
			}

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

	void Engine::Draw(RenderingParams& params) {
		this->Draw(params, mScene.GetMeshes());
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
