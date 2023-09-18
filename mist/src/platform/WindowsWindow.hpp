#pragma once
#include <SDL.h>
#include "../core/Window.hpp"
#include "../core/Application.hpp"

namespace mist {
    class WindowsWindow : public Window {
    public:
        WindowsWindow(const WindowProperties& properties);
        virtual ~WindowsWindow() override;

        WindowsWindow(const WindowsWindow& other) = delete;
        WindowsWindow& operator=(const WindowsWindow& other) = delete;

        inline unsigned int GetWidth() const override { return properties.width; }
		inline unsigned int GetHeight() const override { return properties.height; }

        inline virtual void* GetNativeWindow() const override { return window; }
    private:
        SDL_Window* window;
        WindowProperties properties;
    };
}