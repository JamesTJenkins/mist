#pragma once
#include <memory>
#include "Window.hpp"

namespace mist {
	class Application {
	public:
		Application();
		virtual ~Application();

		void Run();
		void Quit();

		inline Window& GetWindow() { return *window; }
		inline static Application &Get() { return *instance; }
	private:
		bool running = true;

		std::unique_ptr<Window> window;
		static Application *instance;
	};
}