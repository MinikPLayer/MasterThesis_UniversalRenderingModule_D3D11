#pragma once

#include "ITest.h"

#include <URM/Engine/ModelObject.h>
#include <URM/Engine/MeshObject.h>
#include <URM/Engine/FlyCameraObject.h>

#include <directxtk/Keyboard.h>

class SponzaTest : public ITest {
	std::weak_ptr<DirectX::Keyboard> mKeyboard;

	float mLastCameraChange = 0.0f;
	int mCurrentCameraIndex = 0;
	std::vector<std::shared_ptr<URM::Engine::CameraObject>> mCameras;
	std::shared_ptr<URM::Engine::FlyCameraObject> mFlyingCamera;

	std::shared_ptr<URM::Engine::SceneObject> mModel;

	const std::vector<std::pair<Vector3, Vector3>> mCameraTransforms = {
		{{-27, 2.4, 0}, {0, 90, 0}}, // Hallway
		{{-21, 6.6, -11}, {0, 65, 0}}, // Hallway Side
		{{0, 53, 0}, {90, 0, 0}}, // Bird Eye View
		{{-23, 14, -3.5}, {0, 75, 0}} // Floor Center
	};

	// Light transforms
	const std::vector<std::pair<Vector3, Color>> mLightsInformation = {
		{{-5, 40, 20}, Color(1, 0.7, 0.4)}, // Main light
		{{-5, 40, -20}, Color(0.4, 0.7, 1.0)}, // Main light
		{{0, 3, 0}, Color(1, 1, 1)}, // First floor
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
		return URM::Core::WindowCreationParams(640, 480, "URM Vertex Throughput Test", instance);
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
			light->GetTransform().SetPosition(lightInfo.first);
			light->color = lightInfo.second;

			light->diffuseIntensity = 2.0 * light->diffuseIntensity / mLightsInformation.size();
			light->ambientIntensity = 2.0 * light->ambientIntensity / mLightsInformation.size();
		}
	}

	void Init(URM::Engine::Engine& engine) override {
		mKeyboard = engine.GetScene().GetCore().GetKeyboard();

		auto root = engine.GetScene().GetRoot().lock();

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