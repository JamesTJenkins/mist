#pragma once
#include <glm/glm.hpp>
#include <Core.hpp>
#include <renderer/Framebuffer.hpp>
#include <renderer/Buffer.hpp>
#include <renderer/Shader.hpp>
#include <components/Camera.hpp>

namespace mistEditor {
	class SceneWindow {
	public:
		SceneWindow();

		void Initialize();
		void OnEditorUpdate();
		void OnRender();
		void Cleanup();
	private:
		mist::Ref<mist::Shader> testShader;
	};
}