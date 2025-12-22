#include "components/Camera.hpp"
#include "Application.hpp"

namespace mist {
	Camera::Camera(Transform& transform) : transformComponent(transform) {}

	Camera::~Camera() {}

	bool Camera::IsEqual(const Camera& other) const {
		return type == other.type &&
			projectionMatrix == other.projectionMatrix &&
			transformComponent.IsEqual(other.transformComponent) &&
			width == other.width &&
			height == other.height &&
			aspect == other.aspect &&
			size == other.size &&
			orthographicNearPlane == other.orthographicNearPlane &&
			orthographicFarPlane == other.orthographicFarPlane &&
			fov == other.fov &&
			perspectiveNearPlane == other.perspectiveNearPlane &&
			perspectiveFarPlane == other.perspectiveFarPlane;
	}

	void Camera::RecreateCamera() {
		if (type == Orthographic) {
			float left = -size * aspect * 0.5f;
			float right = size * aspect * 0.5f;
			float bottom = -size * 0.5f;
			float top = size * 0.5f;

			projectionMatrix = glm::ortho(left, right, bottom, top, orthographicNearPlane, orthographicFarPlane);
		} else {
			projectionMatrix = glm::perspective(glm::radians(fov), aspect, perspectiveNearPlane, perspectiveFarPlane);
		}

		if (Application::Get().GetRenderAPI()->GetAPI() == RenderAPI::Vulkan)
			projectionMatrix[1][1] *= -1;
	}

	glm::mat4 Camera::GetProjectionMatrix() const {
		return projectionMatrix;
	}

	glm::mat4 Camera::GetViewMatrix() const {
		return glm::lookAt(
			transformComponent.GetPosition(), 
			transformComponent.GetPosition() + transformComponent.Forward(), transformComponent.Up()
		);
	}

	glm::mat4 Camera::GetViewProjectionMatrix() const {
		return projectionMatrix * GetViewMatrix();
	}

	void Camera::SetViewportSize(float _width, float _height) {
		width = _width;
		height = _height;
		aspect = (float)_width / (float)_height;
		RecreateCamera();
	}

	void Camera::SetPerspectiveCamera(const float _width, const float _height, const float _fov, const float _nearPlane, const float _farPlane, const bool isMainCamera) {
		width = _width;
		height = _height;
		aspect = width / height;
		fov = _fov;
		perspectiveNearPlane = _nearPlane;
		perspectiveFarPlane = _farPlane;
		type = Perspective;

		RecreateCamera();
		if (isMainCamera)
			SetMainCamera(this);
	}

	Camera::Camera(const Camera& other) : type(other.type), projectionMatrix(other.projectionMatrix), transformComponent(other.transformComponent), 
		width(other.width), height(other.height), aspect(other.aspect),
		size(other.size), orthographicNearPlane(other.orthographicNearPlane), orthographicFarPlane(other.orthographicFarPlane),
		fov(other.fov), perspectiveNearPlane(other.perspectiveNearPlane), perspectiveFarPlane(other.perspectiveNearPlane) {}

	Camera& Camera::operator=(const Camera& other) {
		if (this == &other)
			return *this;

		type = other.type;
		projectionMatrix = other.projectionMatrix;
		transformComponent = other.transformComponent;
		width = other.width;
		height = other.height;
		aspect = other.aspect;
		size = other.size;
		orthographicNearPlane = other.orthographicNearPlane;
		orthographicFarPlane = other.orthographicFarPlane;
		fov = other.fov;
		perspectiveNearPlane = other.perspectiveNearPlane;
		perspectiveFarPlane = other.perspectiveNearPlane;

		return *this;
	}

	void Camera::SetOrthographicCamera(const float _width, const float _height, const float _size, const float _nearPlane, const float _farPlane, const bool isMainCamera) {
		width = _width;
		height = _height;
		aspect = width / height;
		size = _size;
		orthographicNearPlane = _nearPlane;
		orthographicFarPlane = _farPlane;
		type = Orthographic;

		RecreateCamera();
		if (isMainCamera)
			SetMainCamera(this);
	}
}
