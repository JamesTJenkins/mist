#pragma once
#include "Math.hpp"
#include "physics/IntersectData.hpp"
#include "components/Transform.hpp"
#include "components/Collider.hpp"

namespace mist {
	class Physics {
	public:
		IntersectData DetectCollision(const Transform& transformA, const Collider& colliderA, const Transform& transformB, const Collider& colliderB);
		void Simulate(const float delta);
	};
}