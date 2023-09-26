#pragma once
#include "renderer/RendererAPI.hpp"

namespace mist {
    class Renderer {
    public:
        static void Initialize();
        static void Shutdown();
    private:
        static RendererAPI* rendererAPI;
    };
}