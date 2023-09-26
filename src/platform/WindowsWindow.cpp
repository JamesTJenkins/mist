#include "WindowsWindow.hpp"

namespace mist {
    Window* Window::Create(const WindowProperties& properties) {
        return new WindowsWindow(properties);
    }

    WindowsWindow::WindowsWindow(const WindowProperties& properties) : properties(properties) {
        window = SDL_CreateWindow(
            properties.title.c_str(),
            SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED,
            properties.width,
            properties.height,
            SDL_WINDOW_SHOWN | SDL_WINDOW_VULKAN
        );
    }

    WindowsWindow::~WindowsWindow() {
        SDL_DestroyWindow(window);
    }
}