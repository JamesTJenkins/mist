#pragma once
#include <memory>
#include "core/Core.hpp"
#include "core/Window.hpp"

namespace mist {
	class Application {
	public:
		Application(const char* name = "Untitled");
		virtual ~Application();

		void Run();
		void Quit();

		inline Window& GetWindow() { return *window; }
		inline static Application &Get() { return *instance; }
	private:
		bool running = true;

		std::unique_ptr<Window> window;
		static Application* instance;
	};
}