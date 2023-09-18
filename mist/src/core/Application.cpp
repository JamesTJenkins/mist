#include "Application.hpp"

#define SDL_MAIN_HANDLED
#include <SDL.h>

namespace mist {
    Application* Application::instance = nullptr;

    Application::Application() {
        instance = this;
        SDL_Init(SDL_INIT_VIDEO);

        window = std::unique_ptr<Window>(Window::Create(WindowProperties()));
    }

    Application::~Application() {
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