#pragma once
#include "renderer/RenderAPI.hpp"

namespace mist {
    class MIST_API RenderCommand {
    public:
        inline static void Initialize() { renderAPI->Initialize(); }
        inline static void Shutdown() { renderAPI->Shutdown(); }
        inline static void Clear() { renderAPI->Clear(); }
        inline static void ResizeWindow(uint32_t x, uint32_t y, uint32_t width, uint32_t height) { renderAPI->SetViewport(x, y, width, height); }

        inline static RenderAPI::API GetAPIType() { return renderAPI->GetAPI(); }
        inline static const Scope<RenderAPI>& GetAPI() { return renderAPI; }
    private:
        static Scope<RenderAPI> renderAPI;
    };
}