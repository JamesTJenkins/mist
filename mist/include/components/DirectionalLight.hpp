#pragma once
#include "Math.hpp"

namespace mist {
	struct DirectionalLight {
	public:
		DirectionalLight(Transform& transform, const glm::vec3 lightColor) : transformComponent(transform), lightColor(lightColor) {}

		inline void SetTransform(Transform& value) { transformComponent = value; }
		inline Transform& GetTransform() const { return transformComponent; }

		glm::vec3 lightColor;
	private:
		Transform& transformComponent;
	};
}