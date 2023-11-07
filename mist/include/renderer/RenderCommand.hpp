#pragma once
#include "renderer/RenderAPI.hpp"

namespace mist {
    class RenderCommand {
    public:
        inline static void Initialize() { renderAPI->Initialize(); }
        inline static void Shutdown() { renderAPI->Shutdown(); }

        inline static RenderAPI::API GetAPI() { return renderAPI->GetAPI(); }
    private:
        static RenderAPI* renderAPI;
    };
}