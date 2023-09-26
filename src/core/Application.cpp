#define SDL_MAIN_HANDLED
#include "core/Application.hpp"
#include <SDL.h>
#include "renderer/Renderer.hpp"

namespace mist {
    Application* Application::instance = nullptr;

    Application::Application() {
        instance = this;
        SDL_Init(SDL_INIT_VIDEO);

        window = std::unique_ptr<Window>(Window::Create(WindowProperties()));

        Renderer::Initialize();
    }

    Application::~Application() {
        Renderer::Shutdown();
        SDL_Quit();
    }

    void Application::Run() {
        while (running) {
            SDL_Event event;
			while (SDL_PollEvent (&event)) {
				switch (event.type) {
				case SDL_QUIT:
					Quit();
					break;
				}
			}
        }
    }

    void Application::Quit() {
        running = false;
    }
}