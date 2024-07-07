#define SDL_MAIN_HANDLED

#include "Application.hpp"
#include <SDL.h>
#include "renderer/RenderCommand.hpp"

namespace mist {
    Application* Application::instance{};

    Application::Application(const char* name) {
        if (instance != nullptr)
            throw std::runtime_error("Created 2 application instances");

        instance = this;
        SDL_Init(SDL_INIT_VIDEO);

        window = std::unique_ptr<Window>(Window::Create(WindowProperties(name)));

        RenderCommand::Initialize();
    }

    Application::~Application() {
        RenderCommand::Shutdown();
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

    void Application::PushLayer(Layer* layer) {
        layerStack.PushLayer(layer);
        layer->OnAttach();
    }

    void Application::PopLayer(Layer* layer) {
        layer->OnDetach();
        layerStack.PopLayer(layer);
    }
}