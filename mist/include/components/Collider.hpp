#pragma once
#include <variant>
#include "Math.hpp"

namespace mist {
	struct SphereCollider {
	public:
		SphereCollider(float radius) : radius(radius) {}

		float radius;
	};

	struct BoxCollider {
	public:
		BoxCollider(const glm::vec3 halfExtents) : halfExtents(halfExtents) {}

		glm::vec3 halfExtents;
	};

	struct PlaneCollider {
	public:
		PlaneCollider(const glm::vec3 normal, const float distance) : normal(normal), distance(distance) {}

		glm::vec3 normal;
		float distance;
	};

	struct Collider {
	public:
		std::variant<
			SphereCollider,
			BoxCollider,
			PlaneCollider
		> data;
	};
}