#pragma once
#include "Window.hpp"

namespace mist {
	class LinuxWindow : public Window {
	public:
		LinuxWindow(const WindowProperties& properties);
		virtual ~LinuxWindow() override;

		LinuxWindow(const LinuxWindow& other) = delete;
		LinuxWindow& operator=(const LinuxWindow& other) = delete;
	};
}