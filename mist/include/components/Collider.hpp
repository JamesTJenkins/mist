#pragma once
#include <variant>
#include "Math.hpp"

namespace mist {
	// Sphere collider with a set radius that is affected by scale but will only be scaled by the largest scaling value
	struct SphereCollider {
	public:
		SphereCollider(float radius) : radius(radius) {}

		float radius;
	};

	// Box collider with half extents defining the size of the collider, scaling will affect this
	struct BoxCollider {
	public:
		BoxCollider(const glm::vec3 halfExtents) : halfExtents(halfExtents) {}

		glm::vec3 halfExtents;
	};

	// Plane collider is an infinite plane, scaling has no effect
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