#pragma once
#include "Math.hpp"

namespace mist {
	struct Rigidbody {
	public:
		Rigidbody(const float mass = 1.0f, const float bounce = 0.1f, const glm::vec3 velocity = glm::vec3(0, 0, 0)) : mass(mass), bounce(bounce), velocity(velocity) {}

		float mass;
		float bounce;	// 0 is 0% bounce, 1 is 100% bounce
		glm::vec3 velocity;
	};
}