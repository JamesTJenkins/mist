#pragma once
#include "renderer/RenderAPI.hpp"

namespace mist {
    class MIST_API RenderCommand {
    public:
        inline static void Initialize() { renderAPI->Initialize(); }
        inline static void Shutdown() { renderAPI->Shutdown(); }

        inline static RenderAPI::API GetAPIType() { return renderAPI->GetAPI(); }
        inline static const Scope<RenderAPI>& GetAPI() { return renderAPI; }
    private:
        static Scope<RenderAPI> renderAPI;
    };
}