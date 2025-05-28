#include "pch.h"
#include "Engine.h"
#include "EngineSceneData.h"
#include <URM/Scene/SceneMesh.h>
#include <directxtk/SimpleMath.h>

namespace URM::Engine {
    // TODO: Move these structs to another file.
    struct VertexConstantBuffer
    {
        DirectX::XMMATRIX WVP;
        DirectX::XMMATRIX worldMatrix;
        DirectX::XMMATRIX inverseWorldMatrix;
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

            int __padding__;

            Light(Vector3 position = Vector3::Zero,
                Color color = Color(1, 1, 1),
                float ambient = 0.05f, float diffuse = 0.9f, float specular = 1.0f)
                : color(color.ToVector3()), position(position), ambientIntensity(ambient), diffuseIntensity(diffuse), specularIntensity(specular)
            {}
        };

        alignas(4) Vector4 viewPosition;
        alignas(16) Material material;
        alignas(4) int activeLightsCount = 0;
        alignas(16) Light lights[8];


        PixelConstantBuffer(Vector3 viewPos) : viewPosition(viewPos.x, viewPos.y, viewPos.z, 1.0f) {}
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
        Vector3 cameraPosition,
        Vector3 cameraTarget,
        Vector3 cameraUp,
        float fov, // Degrees
        float nearPlane,
        float farPlane,
        URM::Core::Size2i windowSize
    )
    {
        // 2. View Matrix
        //auto vecCameraPosition = DirectX::XMLoadFloat3(&cameraPosition);
        //auto vecCameraTarget = DirectX::XMLoadFloat3(&cameraTarget);
        //auto vecCameraUp = DirectX::XMLoadFloat3(&cameraUp);
        //auto matView = DirectX::XMMatrixLookAtLH(vecCameraPosition, vecCameraTarget, vecCameraUp);
        URM::Scene::SceneObject cameraObject;
        cameraObject.GetTransform().SetLocalPosition(cameraPosition);
        cameraObject.GetTransform().SetLocalRotation(Quaternion::Identity);
        cameraObject.GetTransform().SetLocalScale(Vector3(1, 1, 1));
        auto matView = cameraObject.GetTransform().GetWorldSpaceMatrix().Invert();

        // 3. Projection Matrix (Orthographic)
        DirectX::XMMATRIX matProjection = DirectX::XMMatrixPerspectiveFovLH(
            DirectX::XMConvertToRadians(fov),
            windowSize.width / (float)windowSize.height,
            nearPlane,
            farPlane
        );

        return WVPMatrix(matProjection, matView, DirectX::XMMatrixIdentity());
    }

    // ============================================================

    void Engine::Update() {
        core.GetWindow().PollEvents();

        timer.Update();
        if (OnUpdate)
            this->OnUpdate(*this);
    }

    void Engine::Present(int vsyncInterval) {
        core.Present(vsyncInterval);
	}

    bool Engine::ShouldClose() {
        return this->core.GetWindow().IsDestroyed();
    }

    void Engine::Clear(Color color) {
        core.Clear(color);
    }

    void Engine::Clear() {
        this->Clear(RenderParameters.clearColor);
    }

    void Engine::Draw(RenderingParams params, std::vector<std::weak_ptr<URM::Scene::SceneMesh>> meshes) {
        auto context = this->core.GetContext();

        auto vp = params.viewport.GetData();
        auto windowSize = this->core.GetWindow().GetSize();
        vp.size = windowSize;
        params.viewport.SetData(vp);
        params.viewport.Bind(this->core);

        params.albedoTextureSampler.Bind(core, 0);

        this->core.SetPrimitiveTopology(params.toplogy);
        params.rasterizerState.Bind(this->core);

        this->vertexConstantBuffer.Bind(this->core, 0);
        this->pixelConstantBuffer.Bind(this->core, 1);

        // TODO: Add dynamic lights with separate type.
        auto rotation = this->timer.GetElapsedTime() * 90.0f;
        auto rotationRad = rotation * DirectX::XM_PI / 180.0f;
        auto cameraPos = Vector3(0.0f, 4.0f, -8.0f);
        const float lightDistance = 2.1f;
        auto lightPosition = Vector3(
            sin(rotationRad) * lightDistance,
            lightDistance / 1.5f,
            cos(rotationRad) * lightDistance
        );

        // TODO: Add support for custom PixelConstantBuffer types.
        auto pixelBufferValue = PixelConstantBuffer(cameraPos);
        pixelBufferValue.activeLightsCount = 3;
        pixelBufferValue.lights[0] = PixelConstantBuffer::Light(
            Vector3(sin(rotationRad) * lightDistance, lightDistance / 1.5f, cos(rotationRad) * lightDistance),
            Color(0, 0, 1),
            0.03f, 0.9f, 1.0f
        );
        pixelBufferValue.lights[1] = PixelConstantBuffer::Light(
            Vector3(sin(rotationRad + 2.1f) * lightDistance, lightDistance / 1.5f, cos(rotationRad + 2.1f) * lightDistance),
            Color(1, 0, 0),
            0.02f, 0.9f, 1.0f
        );
        pixelBufferValue.lights[2] = PixelConstantBuffer::Light(
            Vector3(sin(rotationRad + 4.2f) * lightDistance, lightDistance / 1.5f, cos(rotationRad + 4.2f) * lightDistance),
            Color(0, 1, 0),
            0.01f, 0.9f, 1.0f
        );

        this->pixelConstantBuffer.UpdateWithData(this->core, &pixelBufferValue);

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

			VertexConstantBuffer vcb;
            auto worldMatrix = sceneMesh->GetTransform().GetWorldSpaceMatrix();
            auto meshRenderMatrix = renderMatrix;
            meshRenderMatrix.World = worldMatrix;
            meshRenderMatrix.WVP = worldMatrix * renderMatrix.WVP;
            meshRenderMatrix.Apply(vcb);
            this->vertexConstantBuffer.UpdateWithData(this->core, &vcb);

            sceneMesh->GetInputLayout()->Bind(core);
            sceneMesh->GetShader()->Bind(core);

            auto m = sceneMesh->GetMesh();

            // TODO: Combine similiar meshes to avoid multiple data sending.
            // TODO: Add support for materials.
            if (m.ContainsTextures()) {
                pixelBufferValue.material.useAlbedoTexture = 1;
                m.BindTextures(core);
            }
            else {
                pixelBufferValue.material.useAlbedoTexture = 0;
            }
            this->pixelConstantBuffer.UpdateWithData(this->core, &pixelBufferValue);

            m.GetVertexBuffer().Bind(this->core, 0);
            if (m.ContainsIndices()) {
                m.GetIndexBuffer().Bind(core, 0);

                context->DrawIndexed(m.GetIndicesCount(), 0, 0);
            }
            else {
                m.GetVertexBuffer().Bind(this->core, 0);
                context->Draw(m.GetVerticesCount(), 0);
            }
		}
    }

    void Engine::Draw(RenderingParams params) {
        this->Draw(params, scene.GetMeshes());
    }

    void Engine::RunLoop() {
        while (!core.GetWindow().IsDestroyed()) {
            this->Update();

            this->Clear(RenderParameters.clearColor);
            this->Draw(RenderParameters);
            this->Present(VSyncInterval);
        }
    }

    Engine::Engine(URM::Core::WindowCreationParams windowParams)
        :   core(windowParams), 
            scene(core), 
            vertexConstantBuffer(URM::Core::D3DConstantBuffer::Create<VertexConstantBuffer>(core, URM::Core::ShaderStages::VERTEX)),
            pixelConstantBuffer(URM::Core::D3DConstantBuffer::Create<PixelConstantBuffer>(core, URM::Core::ShaderStages::PIXEL))
    {

    }

}
