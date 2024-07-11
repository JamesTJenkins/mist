#pragma once
#include <vector>
#include <glm/glm.hpp>
#include <Core.hpp>
#include <renderer/FrameBuffer.hpp>

namespace mistEditor {
	class SceneWindow {
	public:
		SceneWindow();

		void OnEditorUpdate();
		void OnRender();
		void SceneWindowDraw();
	
		glm::vec2 viewportSize = {0,0};
	private:
		mist::Ref<mist::FrameBuffer> frameBuffer;
		bool focused = false;
		glm::vec2 viewportBounds[2];
	};
}