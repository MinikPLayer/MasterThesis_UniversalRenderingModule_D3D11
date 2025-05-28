#include "pch.h"
#include "EngineSceneData.h"

std::weak_ptr<URM::Engine::CameraObject> URM::Engine::EngineSceneData::GetMainCamera() {
	return this->mainCamera;
}
