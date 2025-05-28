#pragma once

#include <URM/Scene/SceneObject.h>
#include <directxtk/SimpleMath.h>

namespace URM::Engine {

	// TODO: Light attenuation
	class Light : public URM::Scene::SceneObject {
	public:
		Color color = Color(1, 1, 1);
		float AmbientIntensity = 0.05f;
		float DiffuseIntensity = 0.9f;
		float SpecularIntensity = 1.0f;

	};
}