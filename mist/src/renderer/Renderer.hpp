#pragma once
#include "renderer/RendererAPI.hpp"

namespace mist {
    class Renderer {
    public:
        static void Initialize();
        static void Shutdown();
        
        inline static RendererAPI* GetRendererAPI() { return rendererAPI; }
    private:
        static RendererAPI* rendererAPI;
    };
}