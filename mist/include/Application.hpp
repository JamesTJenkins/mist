#pragma once
#include "Core.hpp"
#include "Window.hpp"
#include "LayerStack.hpp"
#include "imgui/imguiLayer.hpp"

namespace mist {
	class MIST_API Application {
	public:
		Application(const char* name = "Untitled");
		virtual ~Application();

		void Run();
		void Quit();

		void PushLayer(Layer* layer);
		void PopLayer(Layer* layer);

		inline Window& GetWindow() { return *window; }
		inline static Application &Get() { return *instance; }
		ImguiLayer* GetImguiLayer() { return imguiLayer; }
		inline const char* GetApplicationName() { return name; }
	private:
		const char* name;
		bool running = true;
		ImguiLayer* imguiLayer;
		LayerStack layerStack;
		Scope<Window> window;
		static Application* instance;
	};
}