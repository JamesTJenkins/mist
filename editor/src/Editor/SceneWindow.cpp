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
		mist::Framebuffer::Create(properties);

		std::vector<float> verts = {
			-1, 0, 0,
			0, 1, 0,
			1, 0, 0
		};
		vBuffer = mist::VertexBuffer::Create(verts.data(), (uint32_t)verts.size());

		std::vector<uint32_t> indices = {
			0, 1, 2
		};
		iBuffer = mist::IndexBuffer::Create(indices.data(), (uint32_t)indices.size());
	}

	void SceneWindow::OnEditorUpdate() {

	}

	void SceneWindow::OnRender() {
		vBuffer->Bind();
		iBuffer->Bind();
	}

	void SceneWindow::SceneWindowDraw() {
		mist::RenderCommand::Draw();
	}
}