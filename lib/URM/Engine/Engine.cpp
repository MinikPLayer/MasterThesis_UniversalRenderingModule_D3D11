#include "pch.h"
#include "Engine.h"
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
        auto vecCameraPosition = DirectX::XMLoadFloat3(&cameraPosition);
        auto vecCameraTarget = DirectX::XMLoadFloat3(&cameraTarget);
        auto vecCameraUp = DirectX::XMLoadFloat3(&cameraUp);
        auto matView = DirectX::XMMatrixLookAtLH(vecCameraPosition, vecCameraTarget, vecCameraUp);

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

    void Engine::Clear(URM::Core::Color color) {
        core.Clear(color.ToXM());
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
        auto pixelBufferValue = PixelConstantBuffer(
            PixelConstantBuffer::Light(
                1.0f, 1.0f, 1.0f,
                lightPosition.x, lightPosition.y, lightPosition.z,
                0.1f, 0.9f, 1.0f
            ),
            cameraPos
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
    {}

}
