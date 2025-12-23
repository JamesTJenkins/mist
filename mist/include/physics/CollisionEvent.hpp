#pragma once
#include <entt/entt.hpp>

namespace mist {
	class CollisionEvent {
	public:
		CollisionEvent(const entt::entity collidingEntity, const glm::vec3 minimumTranslationVector) : collidingEntity(collidingEntity), minimumTranslationVector(minimumTranslationVector) {}

		entt::entity collidingEntity;
		glm::vec3 minimumTranslationVector;
	};
}