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

        inline virtual void SetGraphicsContext(void* graphicsContext) override { context = graphicsContext; };

        inline unsigned int GetWidth() const override { return properties.width; }
		inline unsigned int GetHeight() const override { return properties.height; }

        inline virtual void* GetNativeWindow() const override { return window; }
        inline virtual void* GetGraphicsContext() const override { return context; }
    private:
        SDL_Window* window;
        WindowProperties properties;
        void* context;
    };
}