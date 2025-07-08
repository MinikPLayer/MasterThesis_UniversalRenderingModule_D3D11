#pragma once

#include "SceneObject.h"
#include <directxtk/SimpleMath.h>

namespace URM::Engine {

	// TODO: Light attenuation
	class LightObject : public SceneObject {
	public:
		Color color = Color(1, 1, 1);
		float ambientIntensity = 0.02f;
		float diffuseIntensity = 1.0f;
		float specularIntensity = 1.0f;

		float attenuationExponent = 2.0f;
		float pbrIntensity = 10.0f;

		void OnAdded() override;
		void OnDestroyed() override;
	};
}
