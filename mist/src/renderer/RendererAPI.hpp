#pragma once
#include <glm/glm.hpp>

namespace mist {
    class RendererAPI {
    public:
        virtual void Initialize() = 0;

        virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) = 0;

        virtual void SetClearColor(glm::vec4& color) = 0;
        virtual void Clear() = 0;
    };
}