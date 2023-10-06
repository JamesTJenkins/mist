#pragma once
#include "Core.hpp"
#include "Window.hpp"

namespace mist {
	class MIST_API Application {
	public:
		Application(const char* name = "Untitled");
		virtual ~Application();

		void Run();
		void Quit();

		inline Window& GetWindow() { return *window; }
		inline static Application &Get() { return *instance; }
	private:
		bool running = true;

		Scope<Window> window;
		static Application* instance;
	};
}