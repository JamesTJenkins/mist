#include "physics/Physics.hpp"
#include "Application.hpp"
#include "Debug.hpp"
#include "components/Rigidbody.hpp"
#include "physics/CollisionEvent.hpp"

namespace mist {
	enum CollisionType {
		SPHERE,
		BOX,
		PLANE
	};

	void Integrate(Transform& transform, Rigidbody& rigidbody, const float delta) {
		transform.SetPosition(transform.GetPosition() + (rigidbody.velocity * delta));
	}

	void ProjectOBB(const Transform& transform, const BoxCollider& collider, glm::vec3 axis, float& min, float& max) {
		glm::mat3 rotMatrix = glm::mat3_cast(transform.GetRotation());
		glm::vec3 corners[] = {
			transform.GetPosition() + rotMatrix * glm::vec3(-collider.halfExtents.x, -collider.halfExtents.y, -collider.halfExtents.z),
			transform.GetPosition() + rotMatrix * glm::vec3( collider.halfExtents.x, -collider.halfExtents.y, -collider.halfExtents.z),
			transform.GetPosition() + rotMatrix * glm::vec3( collider.halfExtents.x,  collider.halfExtents.y, -collider.halfExtents.z),
			transform.GetPosition() + rotMatrix * glm::vec3(-collider.halfExtents.x,  collider.halfExtents.y, -collider.halfExtents.z),
			transform.GetPosition() + rotMatrix * glm::vec3(-collider.halfExtents.x, -collider.halfExtents.y,  collider.halfExtents.z),
			transform.GetPosition() + rotMatrix * glm::vec3( collider.halfExtents.x, -collider.halfExtents.y,  collider.halfExtents.z),
			transform.GetPosition() + rotMatrix * glm::vec3( collider.halfExtents.x,  collider.halfExtents.y,  collider.halfExtents.z),
			transform.GetPosition() + rotMatrix * glm::vec3(-collider.halfExtents.x,  collider.halfExtents.y,  collider.halfExtents.z)
		};

		min = glm::dot(corners[0], axis);
		max = min;
		for (size_t i = 1; i < 8; ++i) {
			float projection = glm::dot(corners[i], axis);
			min = std::min(min, projection);
			max = std::max(max, projection);
		}
	}

	IntersectData SphereIntersect(const Transform& transformA, const SphereCollider& colliderA, const Transform& transformB, const SphereCollider& colliderB) {
		glm::vec3 direction = transformB.GetPosition() - transformA.GetPosition();
		float distanceSqr = glm::dot(direction, direction);
		float radiusSum = colliderA.radius + colliderB.radius;
		float minDistanceSqr = radiusSum * radiusSum;

		if (distanceSqr > minDistanceSqr)
			return IntersectData(false, glm::vec3(0,0,0));

		float distance = std::sqrt(distanceSqr);
		float penetration = radiusSum - distance;
		return IntersectData(true, (direction / distance) * penetration);
	}

	IntersectData SphereBoxIntersect(const Transform& sphereTransform, const SphereCollider& sphereCollider, const Transform& boxTransform, const BoxCollider& boxCollider, const bool invert) {
		glm::mat3 obbRotMatrix = glm::mat3_cast(boxTransform.GetRotation());
		glm::vec3 localSpherePos = glm::inverse(obbRotMatrix) * (sphereTransform.GetPosition() - boxTransform.GetPosition());

		glm::vec3 closestLocalPoint(
			std::max(-boxCollider.halfExtents.x, std::min(localSpherePos.x, boxCollider.halfExtents.x)),
			std::max(-boxCollider.halfExtents.y, std::min(localSpherePos.y, boxCollider.halfExtents.y)),
			std::max(-boxCollider.halfExtents.z, std::min(localSpherePos.z, boxCollider.halfExtents.z))
		);

		glm::vec3 closestWorldPoint = boxTransform.GetPosition() + obbRotMatrix * closestLocalPoint;
		glm::vec3 direction = sphereTransform.GetPosition() - closestWorldPoint;
		float distanceSqr = glm::dot(direction, direction);
		float radiusSqr = sphereCollider.radius * sphereCollider.radius;

		if (distanceSqr > radiusSqr)
			return IntersectData(false, glm::vec3(0,0,0));

		float distance = std::sqrt(distanceSqr);
		float penetration = sphereCollider.radius - distance;
		glm::vec3 mtv = (direction / distance) * penetration;
		return IntersectData(true, invert ? -mtv: mtv);
	}

	IntersectData SpherePlaneIntersect(const Transform& sphereTransform, const SphereCollider& sphereCollider, const Transform& planeTransform, const PlaneCollider& planeCollider, const bool invert) {
		float distance = glm::dot(sphereTransform.GetPosition(), planeCollider.normal) + planeCollider.distance;

		if (std::abs(distance) > sphereCollider.radius)
			return IntersectData(false, glm::vec3(0,0,0));

		float peneration = sphereCollider.radius - std::abs(distance);
		glm::vec3 mtv = planeCollider.normal * (distance > 0 ? -peneration : peneration);
		return IntersectData(true, invert ? -mtv: mtv);
	}

	IntersectData BoxIntersect(const Transform& transformA, const BoxCollider& colliderA, const Transform& transformB, const BoxCollider& colliderB) {
		glm::mat3 rotMatrixA = glm::mat3_cast(transformA.GetRotation());
		glm::mat3 rotMatrixB = glm::mat3_cast(transformB.GetRotation());

		glm::vec3 axisA[] = { rotMatrixA[0], rotMatrixA[1], rotMatrixA[2] };
		glm::vec3 axisB[] = { rotMatrixB[0], rotMatrixB[1], rotMatrixB[2] };

		glm::vec3 axis[] = {
			glm::normalize(axisA[0]),
			glm::normalize(axisA[1]),
			glm::normalize(axisA[2]),
			glm::normalize(axisB[0]),
			glm::normalize(axisB[1]),
			glm::normalize(axisB[2]),
			glm::normalize(glm::cross(axisA[0], axisB[0])),
			glm::normalize(glm::cross(axisA[0], axisB[1])),
			glm::normalize(glm::cross(axisA[0], axisB[2])),
			glm::normalize(glm::cross(axisA[1], axisB[0])),
			glm::normalize(glm::cross(axisA[1], axisB[1])),
			glm::normalize(glm::cross(axisA[1], axisB[2])),
			glm::normalize(glm::cross(axisA[2], axisB[0])),
			glm::normalize(glm::cross(axisA[2], axisB[1])),
			glm::normalize(glm::cross(axisA[2], axisB[2]))
		};

		float minOverlap = FLT_MAX;
		glm::vec3 mtvAxis;

		for (size_t i = 0; i < 15; ++i) {
			if (glm::length(axis[i]) < 1e-6)	// Skip if parallel
				continue;

			float minA, maxA, minB, maxB;
			ProjectOBB(transformA, colliderA, axis[i], minA, maxA);
			ProjectOBB(transformB, colliderB, axis[i], minB, maxB);

			if (maxA < minB || maxB < minA)
				return IntersectData(false, glm::vec3(0,0,0));

			float overlap = std::min(maxA, maxB) - std::max(minA, minB);
			if (overlap < minOverlap) {
				minOverlap = overlap;
				mtvAxis = axis[i];
			}
		}

		return IntersectData(true, mtvAxis * minOverlap);
	}

	IntersectData BoxPlaneIntersect(const Transform& boxTransform, const BoxCollider& boxCollider, const Transform& planeTransform, const PlaneCollider& planeCollider, const bool invert) {
		glm::mat3 boxRotMatrix = glm::mat3_cast(boxTransform.GetRotation());
		glm::vec3 corners[] = {
			boxTransform.GetPosition() + boxRotMatrix * glm::vec3(-boxCollider.halfExtents.x, -boxCollider.halfExtents.y, -boxCollider.halfExtents.z),
			boxTransform.GetPosition() + boxRotMatrix * glm::vec3( boxCollider.halfExtents.x, -boxCollider.halfExtents.y, -boxCollider.halfExtents.z),
			boxTransform.GetPosition() + boxRotMatrix * glm::vec3( boxCollider.halfExtents.x,  boxCollider.halfExtents.y, -boxCollider.halfExtents.z),
			boxTransform.GetPosition() + boxRotMatrix * glm::vec3(-boxCollider.halfExtents.x,  boxCollider.halfExtents.y, -boxCollider.halfExtents.z),
			boxTransform.GetPosition() + boxRotMatrix * glm::vec3(-boxCollider.halfExtents.x, -boxCollider.halfExtents.y,  boxCollider.halfExtents.z),
			boxTransform.GetPosition() + boxRotMatrix * glm::vec3( boxCollider.halfExtents.x, -boxCollider.halfExtents.y,  boxCollider.halfExtents.z),
			boxTransform.GetPosition() + boxRotMatrix * glm::vec3( boxCollider.halfExtents.x,  boxCollider.halfExtents.y,  boxCollider.halfExtents.z),
			boxTransform.GetPosition() + boxRotMatrix * glm::vec3(-boxCollider.halfExtents.x,  boxCollider.halfExtents.y,  boxCollider.halfExtents.z)
		};

		bool allCornersSameSide = true;
		float firstDistance = glm::dot(corners[0], planeCollider.normal) + planeCollider.distance;
		bool firstSide = firstDistance > 0;
		for (size_t i = 1; i < 8; ++i) {
			float distance = glm::dot(corners[i], planeCollider.normal) + planeCollider.distance;
			if ((distance > 0) != firstSide) {
				allCornersSameSide = false;
				break;
			}
		}

		if (allCornersSameSide)
			return IntersectData(false, glm::vec3(0,0,0));

		float minDistance = std::abs(firstDistance);
		for (size_t i = 1; i < 8; ++i) {
			float distance = std::abs(glm::dot(corners[i], planeCollider.normal) + planeCollider.distance);
			minDistance = std::min(minDistance, distance);
		}

		glm::vec3 mtv = planeCollider.normal * minDistance;
		mtv = firstDistance > 0 ? -mtv : mtv;
		return IntersectData(true, invert ? -mtv: mtv);
	}

	IntersectData PlaneIntersect(const Transform& transformA, const PlaneCollider colliderA, const Transform& transformB, const PlaneCollider colliderB) {
		float dotNormal = glm::dot(colliderA.normal, colliderB.normal);

		if (std::abs(dotNormal) < 0.999f)
			return IntersectData(false, glm::vec3(0,0,0));

		glm::vec3 pointOnPlaneA = colliderA.normal * colliderA.distance; 
		float distanceToPlaneB = glm::dot(pointOnPlaneA, colliderB.normal) + colliderB.distance;
		
		if (std::abs(distanceToPlaneB) < 1e-6)
			return IntersectData(true, glm::vec3(0,0,0));

		return IntersectData(false, glm::vec3(0,0,0));
	}

	CollisionType GetCollisionType(const Collider& collider) {
		if (std::holds_alternative<SphereCollider>(collider.data))
			return CollisionType::SPHERE;

		if (std::holds_alternative<BoxCollider>(collider.data))
			return CollisionType::BOX;

		if (std::holds_alternative<PlaneCollider>(collider.data))
			return CollisionType::PLANE;

		MIST_ERROR("Failed to determine collision type, is it implemented?");
		return CollisionType::SPHERE;
	}

	IntersectData Physics::DetectCollision(const Transform& transformA, const Collider& colliderA, const Transform& transformB, const Collider& colliderB) {
		CollisionType aType = GetCollisionType(colliderA);
		CollisionType bType = GetCollisionType(colliderB);

		switch (aType) {
		case CollisionType::SPHERE:
			switch (bType) {
				case CollisionType::SPHERE:	return SphereIntersect(transformA, std::get<SphereCollider>(colliderA.data), transformB, std::get<SphereCollider>(colliderB.data));
				case CollisionType::BOX:	return SphereBoxIntersect(transformA, std::get<SphereCollider>(colliderA.data), transformB, std::get<BoxCollider>(colliderB.data), true);
				case CollisionType::PLANE:	return SpherePlaneIntersect(transformA, std::get<SphereCollider>(colliderA.data), transformB, std::get<PlaneCollider>(colliderB.data), false);
			}
		case CollisionType::BOX:
			switch (bType) {
				case CollisionType::SPHERE:	return SphereBoxIntersect(transformB, std::get<SphereCollider>(colliderB.data), transformA, std::get<BoxCollider>(colliderA.data), false);
				case CollisionType::BOX:	return BoxIntersect(transformA, std::get<BoxCollider>(colliderA.data), transformB, std::get<BoxCollider>(colliderB.data));
				case CollisionType::PLANE:	return BoxPlaneIntersect(transformA, std::get<BoxCollider>(colliderA.data), transformB, std::get<PlaneCollider>(colliderB.data), true);
			}
		case CollisionType::PLANE:
			switch (bType) {
				case CollisionType::SPHERE:	return SpherePlaneIntersect(transformB, std::get<SphereCollider>(colliderB.data), transformA, std::get<PlaneCollider>(colliderA.data), true);
				case CollisionType::BOX:	return BoxPlaneIntersect(transformB, std::get<BoxCollider>(colliderB.data), transformA, std::get<PlaneCollider>(colliderA.data), false);
				case CollisionType::PLANE:	return PlaneIntersect(transformA, std::get<PlaneCollider>(colliderA.data), transformB, std::get<PlaneCollider>(colliderB.data));
			}
		}

		MIST_ERROR("Collider type not implemented");
		return IntersectData(false, glm::vec3(0,0,0));
	}

	void Physics::Simulate(const float delta) {
		entt::registry& scene = Application::Get().GetSceneManager()->GetActiveScene();

		scene.view<Transform, Rigidbody>().each([delta](entt::entity entity, Transform& transform, Rigidbody& rigidbody) {
			Integrate(transform, rigidbody, delta);
		});

		std::unordered_map<entt::entity, std::vector<CollisionEvent>> entityCollisions;
		auto colliderView = scene.view<Transform, Rigidbody, Collider>();
		std::vector<entt::entity> colliderEntities(colliderView.begin(), colliderView.end());

		for (size_t i = 0; i < colliderEntities.size(); ++i) {
			entt::entity a = colliderEntities[i];
			auto [transformA, colliderA] = colliderView.get<Transform, Collider>(a);

			for (size_t j = i + 1; j < colliderEntities.size(); ++j) {
				entt::entity b = colliderEntities[j];
				auto [transformB, colliderB] = colliderView.get<Transform, Collider>(b);
				IntersectData data = DetectCollision(transformA, colliderA, transformB, colliderB);

				if (data.isIntersecting)
					entityCollisions[a].emplace_back(b, data.minimumTranslationVector);
			}
		}

		// Impulse based resolution
		for (auto& [entity, collisions] : entityCollisions) {
			Rigidbody& rigidbody = scene.get<Rigidbody>(entity);
			for (const CollisionEvent& collision : collisions) {
				Rigidbody& otherRigidbody = scene.get<Rigidbody>(collision.collidingEntity);
				glm::vec3 normalizedMTV = glm::normalize(collision.minimumTranslationVector);
				
				glm::vec3 relativeVelocity = otherRigidbody.velocity - rigidbody.velocity;
				float velocityAlongNormal = glm::dot(relativeVelocity, normalizedMTV);

				if (velocityAlongNormal < 0) {
					float j = -(1.0f + std::min(rigidbody.bounce, otherRigidbody.bounce)) * velocityAlongNormal;
					j /= (1.0f / rigidbody.mass + 1.0f / otherRigidbody.mass);

					glm::vec3 impulse = j * normalizedMTV;
					rigidbody.velocity -= impulse / rigidbody.mass;
					otherRigidbody.velocity += impulse / otherRigidbody.mass;
				}

				const float correctionPercent = 0.2f;
				glm::vec3 correction = collision.minimumTranslationVector * correctionPercent;
				Transform& transform = colliderView.get<Transform>(entity);
				Transform& otherTransform = colliderView.get<Transform>(collision.collidingEntity);

				transform.SetPosition(transform.GetPosition() + (correction * (otherRigidbody.mass / (rigidbody.mass + otherRigidbody.mass))));
				otherTransform.SetPosition(otherTransform.GetPosition() - (correction * (rigidbody.mass / (rigidbody.mass + otherRigidbody.mass))));
			}
		}
	}
}