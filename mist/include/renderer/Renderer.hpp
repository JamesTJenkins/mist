#pragma once
#include "renderer/RendererAPI.hpp"

namespace mist {
    class MIST_API Renderer {
    public:
        static void Initialize();
        static void Shutdown();
        
        inline static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }
    private:
        static RendererAPI* rendererAPI;
    };
}