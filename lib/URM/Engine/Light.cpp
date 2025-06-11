#include "pch.h"
#include "Light.h"

#include "Scene.h"

void URM::Engine::Light::OnAdded() {
	auto& scene = this->GetScene();
	auto selfPtr = this->GetSelfPtr().lock();
	auto selfLihtPtr = std::static_pointer_cast<Light>(selfPtr);
	scene.mLights.push_back(selfLihtPtr);
}

void URM::Engine::Light::OnDestroyed() {
	for (size_t i = 0; i < this->GetScene().mLights.size(); ++i) {
		if (this->GetScene().mLights[i].lock().get() == this) {
			this->GetScene().mLights.erase(this->GetScene().mLights.begin() + i);
			break;
		}
	}
}
