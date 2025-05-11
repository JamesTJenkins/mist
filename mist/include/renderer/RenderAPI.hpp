#pragma once
#include <glm/glm.hpp>
#include "Core.hpp"
#include "renderer/Buffer.hpp"

namespace mist {
    class RenderAPI {
    public:
        enum API { None, Vulkan };

        virtual ~RenderAPI() {}

        virtual void Initialize() = 0;
        virtual void Shutdown() = 0;

        virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) = 0;

        virtual void SetClearColor(glm::vec4& color) = 0;
        virtual glm::vec4 GetClearColor() = 0;
        virtual void Clear() = 0;

        virtual void BeginRenderPass() = 0;
        virtual void EndRenderPass() = 0;
        virtual void Draw() = 0;

        virtual API GetAPI() = 0;;
    };
}