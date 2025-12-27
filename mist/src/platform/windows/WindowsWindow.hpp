#pragma once
#include "Window.hpp"

namespace mist {
	class WindowsWindow : public Window {
	public:
		WindowsWindow(const WindowProperties& properties);
		virtual ~WindowsWindow() override;

		WindowsWindow(const WindowsWindow& other) = delete;
		WindowsWindow& operator=(const WindowsWindow& other) = delete;
	};
}