#pragma once
#include <Math.hpp>

// These are default types used for reflection of shaders

namespace mist {
	struct CameraData {
		glm::mat4 u_ViewProjectionMatrix;
	};

	// vec3 in shader is actually 16bytes rather than 12 which means padding is required
	// or it will insert the red value of color into the direction and mess with both
	struct DirectionalLightData {
		glm::vec3 u_LightDir;
		float pad1;
		glm::vec3 u_LightColor;
		float pad2;
	};
}