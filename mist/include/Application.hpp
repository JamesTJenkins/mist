#pragma once
#include <memory>
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
		inline static Application &Get() { return *instance; }	// Make this not exist if you can and that make craig happy
	private:
		bool running = true;

		std::unique_ptr<Window> window;
		static Application* instance;	// Make this not exist also
	};
}