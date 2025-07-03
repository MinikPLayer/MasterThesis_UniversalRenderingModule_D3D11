#pragma once

#include "SceneObject.h"
#include <Windows.h>
#include <directxtk/Keyboard.h>
#include <URM/Engine/CameraObject.h>
#include <URM/Engine/Engine.h>

namespace URM::Engine {
	class FlyCameraObject : public URM::Engine::CameraObject {

		std::weak_ptr<DirectX::Keyboard> mKeyboard;
		float yaw = 0.0f;
		float pitch = 0.0f;

		int GetKeyPressed(char key) const {
			if (auto keyboard = mKeyboard.lock()) {
				return keyboard->GetState().IsKeyDown((DirectX::Keyboard::Keys)key) ? 1 : 0;
			}
			return 0;
		}
	public:
		float mMoveSpeed = 25.f;
		float mRotationSpeed = 3.14f;

		void OnAdded() override {
			CameraObject::OnAdded();
			mKeyboard = GetScene().GetCore().GetKeyboard();
		}

		void OnEngineUpdate(Engine& engine) override {
			auto rotateHorizontal = (GetKeyPressed(VK_RIGHT) - GetKeyPressed(VK_LEFT));
			auto rotateVertical = (GetKeyPressed(VK_DOWN) - GetKeyPressed(VK_UP));

			if (rotateHorizontal != 0 || rotateVertical != 0) {
				auto currentRotation = GetTransform().GetRotation();
				auto newRotationQuat = Quaternion::CreateFromAxisAngle(Vector3::Up, (float)rotateHorizontal * mRotationSpeed * engine.GetTimer().GetDeltaTime());
				currentRotation = currentRotation * newRotationQuat;

				newRotationQuat = Quaternion::CreateFromAxisAngle(GetTransform().GetRightVector(), (float)rotateVertical * mRotationSpeed * engine.GetTimer().GetDeltaTime());
				currentRotation = currentRotation * newRotationQuat;

				GetTransform().SetRotation(currentRotation);
			}
			
			auto horizontal = (GetKeyPressed('D') - GetKeyPressed('A'));
			auto vertical = (GetKeyPressed('W') - GetKeyPressed('S'));
			auto upDown = (GetKeyPressed('E') - GetKeyPressed('Q'));

			if (horizontal != 0 || vertical != 0 || upDown != 0) {
				auto currentPosition = GetTransform().GetPosition();

				currentPosition += GetTransform().GetForwardVector() * (float)vertical * mMoveSpeed * engine.GetTimer().GetDeltaTime();;
				currentPosition += GetTransform().GetRightVector() * (float)horizontal * mMoveSpeed * engine.GetTimer().GetDeltaTime();;
				currentPosition += Vector3::Up * (float)upDown * mMoveSpeed * engine.GetTimer().GetDeltaTime();

				GetTransform().SetPosition(currentPosition);
			}
		}
	};
}