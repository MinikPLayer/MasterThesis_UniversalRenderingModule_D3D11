#pragma once

#include "ITest.h"

#include <URM/Engine/ModelObject.h>
#include <URM/Engine/MeshObject.h>
#include <URM/Engine/FlyCameraObject.h>

#include <directxtk/Keyboard.h>

// Model source: https://www.intel.com/content/www/us/en/developer/topic-technology/graphics-research/samples.html
// + Removed transparent elements.
class SponzaTest : public ITest {
	std::weak_ptr<DirectX::Keyboard> mKeyboard;

	float mLastCameraChange = 0.0f;
	int mCurrentCameraIndex = 0;
	std::vector<std::shared_ptr<URM::Engine::CameraObject>> mCameras;
	std::shared_ptr<URM::Engine::FlyCameraObject> mFlyingCamera;

	std::shared_ptr<URM::Engine::SceneObject> mModel;

	const std::vector<std::pair<Vector3, Vector3>> mCameraTransforms = {
		{{-27, 2.4, 0}, {0, 90, 0}}, // Hallway
		{{-16, 6.0, -11}, {0, 65, 0}}, // Hallway Side
		{{0, 53, 0}, {90, 0, 0}}, // Bird Eye View
		{{-23, 14, -3.5}, {0, 75, 0}} // 1st floor
	};

	// Light transforms
	// Position, Color, Specular intensity, Diffuse intensity, Ambient intensity
	const std::vector<std::tuple<Vector3, Color, float, float, float>> mLightsInformation = {
		{{-20, 40, 20}, Color(1, 0.6, 0.2), 1.0f, 1.0f, 1.0f}, // Main light
		{{-20, 40, -20}, Color(0.2, 0.6, 1.0), 0.5f, 0.5f, 0.5f}, // Main light
		{{0, 3, 0}, Color(1, 1, 1),		  0.5f, 0.05f, 0.05f}, // First floor
		{{ 20,  3,   20}, Color(1, 1, 1), 0.5f, 0.05f, 0.05f}, // First floor
		{{-20,  3,   20}, Color(1, 1, 1), 0.5f, 0.05f, 0.05f}, // First floor
		{{-20,  3,  -20}, Color(1, 1, 1), 0.5f, 0.05f, 0.05f}, // First floor
		{{ 20,  3,  -20}, Color(1, 1, 1), 0.5f, 0.05f, 0.05f}, // First floor
		
		{{ 24,  14,   24}, Color(1, 1, 1), 0.5f, 0.05f, 0.05f}, // Second floor
		{{-24,  14,   24}, Color(1, 1, 1), 0.5f, 0.05f, 0.05f}, // Second floor
		{{-24,  14,  -24}, Color(1, 1, 1), 0.5f, 0.05f, 0.05f}, // Second floor
		{{ 24,  14,  -24}, Color(1, 1, 1), 0.5f, 0.05f, 0.05f}, // Second floor
	};

	UINT CalculateVerticesCount(std::shared_ptr<URM::Engine::ModelObject> model) const {
		auto children = model->GetChildrenByType<URM::Engine::MeshObject>(true);
		UINT count = 0;
		for (const auto& child : children) {
			count += child->GetMesh().GetVerticesCount();
		}

		return count;
	}

public:
	URM::Core::WindowCreationParams GetWindowParams(HINSTANCE instance) const override {
		return URM::Core::WindowCreationParams(1920, 1080, "Sponza", instance);
	}

	void AddCameras(std::shared_ptr<URM::Engine::SceneObject> root) {
		// Cameras setup
		for(auto& transform : mCameraTransforms) {
			auto camera = root->AddChild(new URM::Engine::CameraObject());
			camera->GetTransform().SetLocalPosition(transform.first);
			camera->GetTransform().SetLocalRotation(transform.second);
			mCameras.push_back(camera);
		}
	}

	void AddLights(std::shared_ptr<URM::Engine::SceneObject> root) {
		for (const auto& lightInfo : mLightsInformation) {
			auto light = root->AddChild(new URM::Engine::LightObject());
			light->GetTransform().SetPosition(std::get<0>(lightInfo));
			light->color = std::get<1>(lightInfo);

			auto specularIntensity = std::get<2>(lightInfo);
			auto diffuseIntensity = std::get<3>(lightInfo);
			auto ambientIntensity = std::get<4>(lightInfo);

			light->specularIntensity = specularIntensity * light->specularIntensity;
			light->diffuseIntensity = diffuseIntensity * light->diffuseIntensity;
			light->ambientIntensity = ambientIntensity * light->ambientIntensity;
		}
	}

	void Init(URM::Engine::Engine& engine) override {
		mKeyboard = engine.GetScene().GetCore().GetKeyboard();

		auto root = engine.GetScene().GetRoot().lock();
		engine.renderParameters.clearColor = Color(0.824f, 0.648f, 0.636f);

		// Remove the old camera
		engine.GetScene().GetMainCamera().lock()->Destroy();
		mFlyingCamera = root->AddChild(new URM::Engine::FlyCameraObject());

		const float scale = 2.0f;
		const Vector3 modelSize = { 0, scale, 0 };
		mModel = root->AddChild(new URM::Engine::SceneObject());
		auto model = mModel->AddChild(new URM::Engine::ModelObject("sponza.glb"));
		model->GetTransform().SetLocalScale({ scale, scale, scale });
		model->GetTransform().SetLocalPosition({ -modelSize.x / 2, -modelSize.y / 2, -modelSize.z / 2 });

		auto verticesCount = CalculateVerticesCount(model);
		spdlog::info("[VertexThroughputTest] Model loaded with {} vertices", verticesCount);
	
		AddLights(root);
		AddCameras(root);
		engine.GetScene().SetMainCamera(mCameras[mCurrentCameraIndex]);
	}

	void NextCamera(URM::Engine::Engine& engine) {
		this->mCurrentCameraIndex = (this->mCurrentCameraIndex + 1) % this->mCameras.size();
		engine.GetScene().SetMainCamera(this->mCameras[this->mCurrentCameraIndex]);
	}

	void PrevCamera(URM::Engine::Engine& engine) {
		if (this->mCurrentCameraIndex == 0) {
			this->mCurrentCameraIndex = this->mCameras.size() - 1;
		}
		else {
			this->mCurrentCameraIndex--;
		}

		engine.GetScene().SetMainCamera(this->mCameras[this->mCurrentCameraIndex]);
	}

	bool mPrintPosKeyPressed = false;
	bool mCameraChangeKeyPressed = false;
	void Update(URM::Engine::Engine& engine) override {
		auto keyboard = mKeyboard.lock();
		if (keyboard->GetState().IsKeyDown(DirectX::Keyboard::OemPeriod)) {
			if (!mCameraChangeKeyPressed) {
				NextCamera(engine);
			}

			mCameraChangeKeyPressed = true;
		}
		else if (keyboard->GetState().IsKeyDown(DirectX::Keyboard::OemComma)) {
			if (!mCameraChangeKeyPressed) {
				PrevCamera(engine);
			}
			mCameraChangeKeyPressed = true;
		}
		else if (keyboard->GetState().IsKeyDown(DirectX::Keyboard::O)) {
			engine.GetScene().SetMainCamera(mFlyingCamera);
		}
		else {
			mCameraChangeKeyPressed = false;
		}

		if (keyboard->GetState().IsKeyDown(DirectX::Keyboard::P)) {
			if (!mPrintPosKeyPressed) {
				auto position = mFlyingCamera->GetTransform().GetPosition();
				auto rotation = mFlyingCamera->GetTransform().GetRotationEuler();
				spdlog::info("[SponzaTest] Current camera: \n\t- Position: {}, {}, {}\n\t- Rotation: {}, {}, {}",
					position.x, position.y, position.z,
					rotation.x, rotation.y, rotation.z
				);
			}
			mPrintPosKeyPressed = true;
		}
		else {
			mPrintPosKeyPressed = false;
		}
	}
};