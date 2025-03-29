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
		renderAPI = new VulkanRenderAPI();
		SDL_Init(SDL_INIT_VIDEO);
		window = Window::Create(WindowProperties(name));
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
				case SDL_WINDOWEVENT:
					switch (event.window.event) {
					case SDL_WINDOWEVENT_RESIZED:
						uint32_t newWidth = event.window.data1;
						uint32_t newHeight = event.window.data2;
						if (newWidth > 0 && newHeight > 0 && (window->GetWidth() != newWidth || window->GetHeight() != newHeight))
							RenderCommand::ResizeWindow(window->GetXPosition(), window->GetYPosition(), newWidth, newHeight);
						break;
					}
					break;
				case SDL_QUIT:
					Quit();
					break;
				}

				for (Layer* layer : layerStack) {
					layer->OnEvent(&event);
				}
			}
			
			for (Layer* layer : layerStack) {
				layer->OnUpdate();
			}
			
			window->OnUpdate();

			RenderCommand::BeginRenderPass();
			for (Layer* layer : layerStack) {
				layer->OnRender();
			}
			RenderCommand::EndRenderPass();
			RenderCommand::Draw();
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