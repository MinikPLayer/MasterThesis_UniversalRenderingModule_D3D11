#include "pch.h"
#include "Engine.h"
#include "MeshObject.h"
#include "ConstantBufferTypes.h"
#include <URM/Core/Stopwatch.h>
#include <URM/Core/Log.h>

#undef min

namespace URM::Engine {
	struct WVPMatrix {
		Matrix wvp;
		Matrix world;

		void Apply(VertexConstantBufferData& buffer) const {
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

		mScene.GetRoot().lock()->RunEventRecursively([this](SceneObject* object) {
			object->OnEngineUpdate(*this);
		});

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
		mCore.ClearFramebuffer(color);
	}

	void Engine::Clear() {
		this->Clear(renderParameters.clearColor);
	}

	void Engine::Draw(RenderingParams& params, std::weak_ptr<CameraObject> mainCamera, std::vector<std::weak_ptr<MeshObject>>& meshes, std::vector<std::weak_ptr<LightObject>>& lights) {
		if (mainCamera.expired()) {
			throw std::runtime_error("Main camera is not set. Cannot draw the scene.");
		}
		
		if(lights.size() == 0 && !mNoLightsWarningShown) {
			spdlog::warn("No lights in the scene - high chance of a black screen. This warning won't be shown again in this session.");
			mNoLightsWarningShown = true;
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

		this->mVertexConstantBuffer.Bind(this->mCore, VertexConstantBufferData::SEMANTIC_SHADER_CONSTANT_BUFFER_INDEX);
		this->mPixelConstantBuffer.Bind(this->mCore, PixelConstantBufferData::SEMANTIC_SHADER_CONSTANT_BUFFER_INDEX);
		this->mPixelLightsConstantBuffer.Bind(this->mCore, PixelLightBufferData::SEMANTIC_SHADER_CONSTANT_BUFFER_INDEX);

		params.geometryPassParams.blendState.Bind(this->mCore);
		params.geometryPassParams.depthStencilState.Bind(this->mCore);

		auto cameraPos = cameraPtr->GetTransform().GetPosition();
		auto renderMatrix = CreateTransformationMatrix(cameraPtr.get(), windowSize);

		auto pixelBufferValue = PixelConstantBufferData(cameraPos);
		this->mPixelConstantBuffer.UpdateWithData(this->mCore, &pixelBufferValue);

		// TODO: Add support for custom PixelConstantBuffer types.
		auto lightsBufferValue = PixelLightBufferData();
		size_t allLightsCount = lights.size();
		for (int lightOffset = 0; lightOffset < allLightsCount; lightOffset += PixelLightBufferData::MAX_LIGHTS_COUNT) {
			if (lightOffset != 0) {
				params.lightingPassParams.blendState.Bind(this->mCore);
				params.lightingPassParams.depthStencilState.Bind(this->mCore);
			}
			
			size_t lightsCount = std::min((size_t)PixelLightBufferData::MAX_LIGHTS_COUNT, allLightsCount - lightOffset);
			lightsBufferValue.activeLightsCount = static_cast<uint32_t>(lightsCount);
			for (size_t i = 0; i < lightsCount; i++) {
				auto l = lights[lightOffset + i].lock();
				lightsBufferValue.lights[i] = PixelLight(
					l->GetTransform().GetPosition(),
					l->color,
					l->ambientIntensity,
					l->diffuseIntensity,
					l->specularIntensity,
					l->attenuationExponent,
					l->pbrIntensity
				);
			}

			this->mPixelLightsConstantBuffer.UpdateWithData(this->mCore, &lightsBufferValue);
			// TODO: Group meshes by shaders and input layouts.
			for (auto& mesh : meshes) {
				auto sceneMesh = mesh.lock();

				VertexConstantBufferData vcb;
				auto worldMatrix = sceneMesh->GetTransform().GetWorldSpaceMatrix();
				auto meshRenderMatrix = renderMatrix;
				meshRenderMatrix.world = worldMatrix;
				meshRenderMatrix.wvp = worldMatrix * renderMatrix.wvp;
				meshRenderMatrix.Apply(vcb);
				this->mVertexConstantBuffer.UpdateWithData(this->mCore, &vcb);

				sceneMesh->GetInputLayout()->Bind(mCore);
				sceneMesh->GetVertexShader()->Bind(mCore);

				auto& m = sceneMesh->GetMesh();

				// TODO: Combine similiar meshes to avoid multiple data sending.
				bool useTexture = false;
				if (m.ContainsTextures()) {
					useTexture = true;
					m.BindTextures(mCore);
				}
				sceneMesh->material->Bind(mCore, URM::Core::Material::SEMANTIC_SHADER_CONSTANT_BUFFER_INDEX);
				sceneMesh->material->Prepare(mCore, useTexture);

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
		//if (lightsCount > PixelConstantBuffer::MAX_LIGHTS_COUNT) {
		//	spdlog::warn("Too many lights in the scene. Max supported: {}, current: {}. Truncating to the max value.", PixelConstantBuffer::MAX_LIGHTS_COUNT, lightsCount);
		//	lightsCount = PixelConstantBuffer::MAX_LIGHTS_COUNT;
		//}
		
	}

	void Engine::Draw(RenderingParams& params, Scene& scene) {
		this->Draw(params, mScene.GetMainCamera(), mScene.GetMeshes(), scene.GetLights());
	}
	
	void Engine::Draw(RenderingParams& params) {
		this->Draw(params, mScene);
	}
	
	void Engine::RunLoopTrace() {
		Core::Stopwatch stopwatch;
		auto lastUpdate = 0.0f;

		stopwatch.PreallocateTimePoints(4);
		while (!mCore.GetWindow().IsDestroyed()) {
			stopwatch.Reset();
			
			this->Update();
			stopwatch.AddPoint("Engine::Update");

			this->Clear(renderParameters.clearColor);
			stopwatch.AddPoint("Engine::ClearFramebuffer");
			
			this->Draw(renderParameters);
			stopwatch.AddPoint("Engine::Draw");
	
			this->Present(vSyncInterval);
			stopwatch.AddPoint("Engine::Present");

			auto now = mTimer.GetElapsedTime();
			if (now - lastUpdate > 1.0f) {
				auto result = stopwatch.GetResult();
				spdlog::trace(result.ToString("Engine::RunLoop()"));
				lastUpdate = now;
			}
		}
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
	                                                                 mVertexConstantBuffer(Core::D3DConstantBuffer::Create<VertexConstantBufferData>(mCore, Core::ShaderStages::VERTEX)),
	                                                                 mPixelConstantBuffer(Core::D3DConstantBuffer::Create<PixelConstantBufferData>(mCore, Core::ShaderStages::PIXEL)),
	                                                                 mPixelLightsConstantBuffer(Core::D3DConstantBuffer::Create<PixelLightBufferData>(mCore, Core::ShaderStages::PIXEL))
	{
		URM::Core::Logger::InitLogger();

		auto lightPassBlendStateData = this->renderParameters.lightingPassParams.blendState.GetData();
		lightPassBlendStateData.enableBlending = true;
		lightPassBlendStateData.srcBlend = D3D11_BLEND_ONE;
		lightPassBlendStateData.destBlend = D3D11_BLEND_ONE;
		lightPassBlendStateData.blendOp = D3D11_BLEND_OP_ADD;
		this->renderParameters.lightingPassParams.blendState.SetData(lightPassBlendStateData);

		auto lightPassDepthStencilStateData = this->renderParameters.lightingPassParams.depthStencilState.GetData();
		lightPassDepthStencilStateData.depthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
		lightPassDepthStencilStateData.depthFunc = D3D11_COMPARISON_EQUAL;
		this->renderParameters.lightingPassParams.depthStencilState.SetData(lightPassDepthStencilStateData);
	}

}
