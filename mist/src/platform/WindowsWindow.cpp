#include "platform/WindowsWindow.hpp"
#include "renderer/RenderCommand.hpp"

namespace mist {
    Window* Window::Create(const WindowProperties& properties) {
        return new WindowsWindow(properties);
    }

    WindowsWindow::WindowsWindow(const WindowProperties& properties) : properties(properties) {
        uint32_t flags = 0;
        switch (RenderCommand::GetAPI()) {
            case RenderAPI::API::Vulkan:
                flags = SDL_WINDOW_SHOWN | SDL_WINDOW_VULKAN;
                break;
            default:
                break;
        }

        window = SDL_CreateWindow(
            properties.title.c_str(),
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
}