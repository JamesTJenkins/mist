#define SDL_MAIN_HANDLED

#include "Application.hpp"
#include <SDL.h>
#include <stdexcept>
#include "renderer/RenderCommand.hpp"
#include "renderer/vulkan/VulkanRenderAPI.hpp"
#include "Log.hpp"

namespace mist {
	Application* Application::instance = nullptr;

	Application::Application(const char* name) {
		Log::Init();

		if (instance != nullptr)
			throw std::runtime_error("Created 2 application instances");

		instance = this;
		appName = name;
		renderAPI = std::unique_ptr<VulkanRenderAPI>(new VulkanRenderAPI());
		SDL_Init(SDL_INIT_VIDEO);
		window = std::unique_ptr<Window>(Window::Create(WindowProperties(name)));
		RenderCommand::Initialize();
	}

	Application::~Application() {
		RenderCommand::Shutdown();
		SDL_Quit();
	}

	Application& Application::Get() {
		if (instance == nullptr)
			instance = new Application();
		
		return *instance;
	}

	void Application::Run() {
		while (running) {
			SDL_Event event;
			while (SDL_PollEvent (&event)) {
				switch (event.type) {
				case SDL_WINDOWEVENT_RESIZED:
					RenderCommand::ResizeWindow(window->GetXPosition(), window->GetYPosition(), window->GetWidth(), window->GetHeight());
					break;
				case SDL_WINDOWEVENT_CLOSE:
				case SDL_QUIT:
					Quit();
					break;
				}

				for (Layer* layer : layerStack) {
					layer->OnEvent(&event);
				}
			}
		}

		for (Layer* layer : layerStack) {
			layer->OnUpdate();
		}

		imguiLayer->Begin();
		for (Layer* layer : layerStack) {
            layer->OnRender();
        }
		imguiLayer->End();

		window->OnUpdate();
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