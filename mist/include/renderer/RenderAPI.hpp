#pragma once
#include <Math.hpp>
#include "Core.hpp"
#include "renderer/Buffer.hpp"
#include "components/Camera.hpp"
#include "components/MeshRenderer.hpp"

namespace mist {
    class RenderAPI {
    public:
        enum API { None, Vulkan };
        enum VSYNC { Off, On, TripleBuffer };

        virtual ~RenderAPI() {}

        virtual void Initialize() = 0;
        virtual void Shutdown() = 0;

        virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) = 0;

        virtual void SetClearColor(glm::vec4& color) = 0;
        virtual glm::vec4 GetClearColor() = 0;

        virtual void WaitForIdle() = 0;
        virtual void BeginRenderPass() = 0;
        virtual void EndRenderPass() = 0;
        virtual void UpdateCamera(Camera& camera) = 0;
        virtual void BindMeshRenderer(const MeshRenderer& MeshRenderer) = 0;
        virtual void Draw(uint32_t indexCount) = 0;

        virtual API GetAPI() = 0;

        virtual void SetVsyncMode(RenderAPI::VSYNC newMode) = 0;
        virtual VSYNC GetVsyncMode() = 0;
    };
}