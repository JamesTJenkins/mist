#pragma once
#include "Core.hpp"
#include "Window.hpp"
#include "LayerStack.hpp"
#include <renderer/RenderAPI.hpp>
#include <renderer/Shader.hpp>

namespace mist {
	class MIST_API Application {
	public:
		Application(const char* name = "Untitled");
		virtual ~Application();

		Application(const Application&) = delete;
		Application& operator=(const Application&) = delete;

		static Application& Get();

		void Run();
		void Quit();

		void PushLayer(Layer* layer);
		void PopLayer(Layer* layer);

		inline RenderAPI& GetRenderAPI() { return *renderAPI; }
		inline Window& GetWindow() { return *window; }
		inline const char* GetApplicationName() { return appName; }
		inline ShaderLibrary* GetShaderLibrary() { return &shaderLib; }
	private:
		const char* appName = "Untitled";
		bool running = true;
		LayerStack layerStack;
		ShaderLibrary shaderLib;
		Scope<Window> window;
		Scope<RenderAPI> renderAPI;
		static Application* instance;
	};
}