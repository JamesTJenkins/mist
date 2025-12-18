#pragma once
#include <stdint.h>
#include <string>
#include <Math.hpp>
#include <SDL3/SDL.h>
#include "Core.hpp"

namespace mist {
	struct WindowProperties {
		const char* title;
		uint32_t width, height;

		WindowProperties(const char* _title = "Untitled Window", uint32_t width = 1280, uint32_t height = 720) : title(_title), width(width), height(height) {}
	};

	class Window {
	public:
		virtual ~Window(){};

		virtual SDL_Window* GetNativeWindow() const = 0;

		virtual void OnUpdate() = 0;

		virtual glm::vec2 GetWindowPosition() const = 0;
		virtual uint32_t GetXPosition() const = 0;
		virtual uint32_t GetYPosition() const = 0;
		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;

		static Window* Create(const WindowProperties& properties = WindowProperties());
	};
}