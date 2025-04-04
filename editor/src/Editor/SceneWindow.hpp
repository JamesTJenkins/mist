#pragma once
#include <glm/glm.hpp>
#include <Core.hpp>
#include <renderer/Framebuffer.hpp>
#include <renderer/Buffer.hpp>
#include <renderer/Shader.hpp>

namespace mistEditor {
	class SceneWindow {
	public:
		SceneWindow();

		void OnEditorUpdate();
		void OnRender();
		void SceneWindowDraw();
	private:
		mist::Ref<mist::Shader> testShader;
		mist::Ref<mist::VertexBuffer> vBuffer;
		mist::Ref<mist::IndexBuffer> iBuffer;
	};
}