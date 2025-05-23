#pragma once
#include "renderer/RenderAPI.hpp"
#include "Application.hpp"

namespace mist {
    class RenderCommand {
    public:
        inline static void Initialize() { Application::Get().GetRenderAPI()->Initialize(); }
        inline static void Shutdown() { Application::Get().GetRenderAPI()->Shutdown(); }
        inline static void Clear() { Application::Get().GetRenderAPI()->Clear(); }
        inline static void ResizeWindow(uint32_t x, uint32_t y, uint32_t width, uint32_t height) { Application::Get().GetRenderAPI()->SetViewport(x, y, width, height); }
        inline static void BeginRenderPass() { Application::Get().GetRenderAPI()->BeginRenderPass(); }
        inline static void EndRenderPass() { Application::Get().GetRenderAPI()->EndRenderPass(); }
        inline static void Draw() { Application::Get().GetRenderAPI()->Draw(); }

        inline static RenderAPI::API GetAPIType() { return Application::Get().GetRenderAPI()->GetAPI(); }
        inline static RenderAPI* GetAPI() { return Application::Get().GetRenderAPI(); }
    };
}