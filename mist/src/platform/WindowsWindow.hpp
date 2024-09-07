#pragma once
#include <SDL.h>
#include <vulkan/vulkan.hpp>
#include "Window.hpp"
#include "Application.hpp"

namespace mist {
    class WindowsWindow : public Window {
    public:
        WindowsWindow(const WindowProperties& properties);
        virtual ~WindowsWindow() override;

        WindowsWindow(const WindowsWindow& other) = delete;
        WindowsWindow& operator=(const WindowsWindow& other) = delete;

        virtual void OnUpdate() override;

        inline virtual glm::vec2 GetWindowPosition() const override;
        inline virtual uint32_t GetXPosition() const override { return (uint32_t)GetWindowPosition().x; }
        inline virtual uint32_t GetYPosition() const override { return (uint32_t)GetWindowPosition().y; }
        inline virtual uint32_t GetWidth() const override { return properties.width; }
		inline virtual uint32_t GetHeight() const override { return properties.height; }

        inline virtual void* GetNativeWindow() const override { return window; }
    private:
        SDL_Window* window;
        WindowProperties properties;
    };
}