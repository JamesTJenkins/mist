#pragma once
#include <typeinfo>
#include "Components/Transform.hpp"

namespace mist {
    class Camera {
    public:
        enum ProjectionType { Perspective = 0, Orthographic = 1 };
    public:
        // BOTH
        Camera() = default;
        Camera(Transform* transform);
        virtual ~Camera();

        bool IsEqual(const Camera& other) const;

        void RecreateCamera();

        glm::mat4 GetProjectionMatrix() const;
        glm::mat4 GetViewMatrix() const;
        glm::mat4 GetViewProjectionMatrix() const;
        
        ProjectionType GetProjectionType() const { return type; }
        void SetProjectionType(ProjectionType value) { type = value; RecreateCamera(); };

        void SetViewportSize(float width, float height);

        float GetCameraWidth() const { return width; }
        float GetCameraHeight() const { return height; }

        inline void SetTransform(Transform* value) { transformComponent = value; }
        inline Transform* GetTransform() const { return transformComponent; }

        inline static Camera* GetMainCamera() { return mainCamera; }
        inline static void SetMainCamera(Camera* value) { mainCamera = value; }

        // ORTHOGRAPHIC
        void SetOrthographicCamera(const float width, const float height, const float size = 10, const float nearPlane = -1, const float farPlane = 1, const bool isMainCamera = true);
        
        float GetOrthographicSize() const { return size; }
        void SetOrthographicSize(float value) { size = value; RecreateCamera(); }

        float GetOrthographicNearPlane() const { return orthographicNearPlane; }
        void SetOrthographicNearPlane(float value) { orthographicNearPlane = value; RecreateCamera(); }

        float GetOrthographicFarPlane() const { return orthographicFarPlane; }
        void SetOrthographicFarPlane(float value) { orthographicFarPlane = value; RecreateCamera(); }

        // PERSPECTIVE
        void SetPerspectiveCamera(const float width, const float height, const float fov = 60, const float nearPlane = 0.1f, const float farPlane = 1000, const bool isMainCamera = true);

        float GetPerspectiveFOV() const { return fov; }
        void SetPerspectiveFOV(float value) { fov = value; RecreateCamera(); }

        float GetPerspectiveNearPlane() const { return perspectiveNearPlane; }
        void SetPerspectiveNearPlane(float value) { perspectiveNearPlane = value; RecreateCamera(); }

        float GetPerspectiveFarPlane() const { return perspectiveFarPlane; }
        void SetPerspectiveFarPlane(float value) { perspectiveFarPlane = value; RecreateCamera(); }
    public:
        Camera(const Camera& other);				// Copy construct
        Camera& operator=(const Camera& other);	    // Copy assign

        bool operator==(const Camera& other) const { return typeid(*this) == typeid(other) && IsEqual(other); }
    private:
        // General camera
        ProjectionType type;
        glm::mat4 projectionMatrix;
        Transform* transformComponent;

        float width;
        float height;
        float aspect;
    private:
        // Orthographic
        float size = 10;
        float orthographicNearPlane = -1;
        float orthographicFarPlane = 1;
    private:
        // Perspective
        float fov = 60;
        float perspectiveNearPlane = 0.1f;
        float perspectiveFarPlane = 1000;
    private:
        // TODO: Move to scene
        inline static Camera* mainCamera;
    };
}