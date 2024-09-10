#pragma once
#include <vector>
#include <glm/glm.hpp>
#include <Core.hpp>
#include <renderer/Framebuffer.hpp>

namespace mistEditor {
	class SceneWindow {
	public:
		SceneWindow();

		void OnEditorUpdate();
		void OnRender();
		void SceneWindowDraw();
	
		glm::vec2 viewportSize = {0,0};
	private:
		mist::Ref<mist::Framebuffer> framebuffer;
		bool focused = false;
		glm::vec2 viewportBounds[2];
	};
}