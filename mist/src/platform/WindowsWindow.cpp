#include "platform/WindowsWindow.hpp"

namespace mist {
    Window* Window::Create(const WindowProperties& properties) {
        return new WindowsWindow(properties);
    }

    WindowsWindow::WindowsWindow(const WindowProperties& properties) : properties(properties) {
        SDL_WindowFlags flags = 0;
        switch (Application::Get().GetRenderAPI()->GetAPI()) {
            case RenderAPI::API::Vulkan:
                flags = SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY;
                break;
            default:
                break;
        }

        window = SDL_CreateWindow(
            properties.title,
            properties.width,
            properties.height,
            flags
        );
    }

    WindowsWindow::~WindowsWindow() {
        SDL_DestroyWindow(window);
    }

    void WindowsWindow::OnUpdate() {
        
    }

    glm::vec2 WindowsWindow::GetWindowPosition() const {
        int x, y;
        SDL_GetWindowPosition(window, &x, &y);
        return glm::vec2(x, y);
    }
}