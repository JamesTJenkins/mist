#pragma once
#include <imgui_impl_vulkan.h>
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
        virtual void OnRender() override;

        void Begin();
        void End();

        ImGuiContext* GetContext() { return imguiContext; }
    private:
        void SetDarkThemeColors();

        ImGuiContext* imguiContext;
        ImGui_ImplVulkanH_Window window;
    };
}