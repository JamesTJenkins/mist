#pragma once
#include <stdint.h>
#include <string>
#include "Core.hpp"

namespace mist {
	struct MIST_API WindowProperties {
		std::string title;
		uint32_t width, height;

		WindowProperties(const std::string &title = "Untitled Window", uint32_t width = 1280, uint32_t height = 720) : title(title), width(width), height(height) {}
	};

	class MIST_API Window {
	public:
		virtual ~Window(){};

		virtual void* GetNativeWindow() const = 0;

		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;

		static Window* Create(const WindowProperties& properties = WindowProperties());
	};
}