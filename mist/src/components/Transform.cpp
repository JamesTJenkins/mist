#include "components/Transform.hpp"

namespace mist {
	Transform::Transform(glm::vec3 position, glm::quat rotation, glm::vec3 scale) : position(position), rotation(rotation), scale(scale) {}

	bool Transform::IsEqual(const Transform& other) const {
		return position == other.position &&
			rotation == other.rotation &&
			scale == other.scale;
	}

	void Transform::Rotate(float angle_in_radians, glm::vec3 axis) {
		rotation *= glm::angleAxis(angle_in_radians, glm::normalize(axis));
	}

	glm::quat Transform::EulerToQuat(glm::vec3 rotation_in_degrees) {
		return glm::quat(glm::radians(rotation_in_degrees));
	}

	glm::vec3 Transform::QuatToEuler(glm::quat quaternion) {
		return glm::degrees(glm::eulerAngles(quaternion));
	}

	void Transform::SetPosition(glm::vec3 position) { this->position = position; }
	void Transform::SetRotation(glm::quat rotation) { this->rotation = rotation; }
	void Transform::SetScale(glm::vec3 scale) { this->scale = scale; }

	glm::vec3 Transform::GetPosition() const { return position; }
	glm::quat Transform::GetRotation() const { return rotation; }
	glm::vec3 Transform::GetScale() const { return scale; }

	glm::vec3 Transform::Left() const { return rotation * glm::vec3(1.0f, 0.0f, 0.0f); }
	glm::vec3 Transform::Right() const { return rotation * glm::vec3(-1.0f, 0.0f, 0.0f); }
	glm::vec3 Transform::Up() const { return rotation * glm::vec3(0.0f, 1.0f, 0.0f); }
	glm::vec3 Transform::Down() const { return rotation * glm::vec3(0.0f, -1.0f, 0.0f); }
	glm::vec3 Transform::Forward() const { return rotation * glm::vec3(0.0f, 0.0f, 1.0f); }
	glm::vec3 Transform::Backward() const { return rotation * glm::vec3(0.0f, 0.0f, -1.0f); }

	glm::mat4 Transform::GetLocalToWorldMatrix() const {
		glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), position);
		modelMatrix *= glm::mat4_cast(rotation);
		return glm::scale(modelMatrix, scale);
	}

	glm::mat4 Transform::GetWorldToLocalMatrix() const {
		glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), -position);
		glm::mat4 rotationMatrix = glm::mat4_cast(glm::inverse(rotation));
		glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), 1.0f / scale);

		return scaleMatrix * rotationMatrix * translationMatrix;
	}
}