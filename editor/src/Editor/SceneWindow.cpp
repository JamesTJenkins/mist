#include "SceneWindow.hpp"
#include <renderer/RenderCommand.hpp>

namespace mistEditor {
	SceneWindow::SceneWindow() {
		mist::FramebufferProperties properties;
		properties.attachment = { 
			mist::FramebufferTextureFormat::RGBA8,
			mist::FramebufferTextureFormat::R8,
			mist::FramebufferTextureFormat::DEPTH24_STENCIL8
		};
		properties.width = 1280;
		properties.height = 720;
		framebuffer = mist::Framebuffer::Create(properties);
	}

	void SceneWindow::OnEditorUpdate() {

	}

	void SceneWindow::OnRender() {

	}

	void SceneWindow::SceneWindowDraw() {
		mist::RenderCommand::Draw();
	}
}