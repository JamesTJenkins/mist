#pragma once
#include "Math.hpp"

namespace mist {
	class IntersectData {
	public:
		IntersectData(const bool isIntersecting, const glm::vec3 minimumTranslationVector) : isIntersecting(isIntersecting), minimumTranslationVector(minimumTranslationVector) {}

		const bool isIntersecting;
		const glm::vec3 minimumTranslationVector;   // Smallest vector that seperates two colliders with the length being the penetration depth
	};
}