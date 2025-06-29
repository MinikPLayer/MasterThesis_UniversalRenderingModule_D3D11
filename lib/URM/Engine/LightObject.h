#pragma once

#include "SceneObject.h"
#include <directxtk/SimpleMath.h>

namespace URM::Engine {

	// TODO: Light attenuation
	class LightObject : public SceneObject {
	public:
		Color color = Color(1, 1, 1);
		float ambientIntensity = 0.05f;
		float diffuseIntensity = 0.9f;
		float specularIntensity = 1.0f;

		void OnAdded() override;
		void OnDestroyed() override;
	};
}
