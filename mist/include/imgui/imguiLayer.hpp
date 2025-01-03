#pragma once
#include "Layer.hpp"

namespace mist {
    class MIST_API ImguiLayer : public Layer {
    public:
        ImguiLayer(const char* name = "Layer");
        ~ImguiLayer();

        ImguiLayer(const ImguiLayer& other) = delete;
        ImguiLayer& operator=(const ImguiLayer& other) = delete;

        virtual void OnAttach() override;
        virtual void OnDetach() override;
        virtual void OnUpdate() override;
        virtual void OnEvent(const SDL_Event* e) override;

        void Begin();
        void End();
    private:
        void SetDarkThemeColors();
    };
}