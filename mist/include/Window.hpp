#pragma once
#include <Math.hpp>
#include <SDL3/SDL.h>

namespace mist {
	struct WindowProperties {
		const char* title;
		uint32_t width, height;

		WindowProperties(const char* _title = "Untitled Window", uint32_t width = 1280, uint32_t height = 720) : title(_title), width(width), height(height) {}
	};

	class Window {
	public:
		virtual ~Window(){};

		const glm::vec2 GetWindowPosition() const;
		const uint32_t GetXPosition() const;
		const uint32_t GetYPosition() const;

		uint32_t GetWidth() const { return properties.width; }
		uint32_t GetHeight() const { return properties.height; }

		inline SDL_Window* GetNativeWindow() const{ return window; }

		static Window* Create(const WindowProperties& properties = WindowProperties());
	protected:
		WindowProperties properties;
		SDL_Window* window;
	};
}