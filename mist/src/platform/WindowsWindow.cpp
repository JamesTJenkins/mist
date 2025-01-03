#include "platform/WindowsWindow.hpp"
#include "renderer/RenderCommand.hpp"

namespace mist {
    Window* Window::Create(const WindowProperties& properties) {
        return new WindowsWindow(properties);
    }

    WindowsWindow::WindowsWindow(const WindowProperties& properties) : properties(properties) {
        uint32_t flags = 0;
        switch (RenderCommand::GetAPIType()) {
            case RenderAPI::API::Vulkan:
                flags = SDL_WINDOW_SHOWN | SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI;
                break;
            default:
                break;
        }

        window = SDL_CreateWindow(
            properties.title,
            SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED,
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