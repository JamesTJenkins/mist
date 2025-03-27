#pragma once
#include <vector>
#include <glm/glm.hpp>
#include <Core.hpp>
#include <renderer/Framebuffer.hpp>
#include <renderer/Buffer.hpp>

namespace mistEditor {
	class SceneWindow {
	public:
		SceneWindow();

		void OnEditorUpdate();
		void OnRender();
		void SceneWindowDraw();
	private:
		mist::Ref<mist::Framebuffer> framebuffer;
		mist::Ref<mist::VertexBuffer> vBuffer;
		mist::Ref<mist::IndexBuffer> iBuffer;
	};
}