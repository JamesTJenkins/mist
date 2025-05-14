#include "SceneWindow.hpp"
#include <renderer/Buffer.hpp>
#include <renderer/RenderCommand.hpp>
#include <vector>

namespace mistEditor {
	SceneWindow::SceneWindow() : sceneCameraTransform(), sceneCamera(&sceneCameraTransform) {
		std::vector<mist::FramebufferTextureProperties> attachments = {
			mist::FramebufferTextureFormat::RGBA8
		};
		mist::FramebufferProperties properties;
		properties.attachment.attachments = attachments.data();
		properties.attachment.attachmentsCount = (uint32_t)attachments.size();
		properties.width = 1280;
		properties.height = 720;
		mist::Framebuffer::Create(properties);

		std::vector<mist::Vertex> verts = {
			{{  0.0f, -0.5f, 0.0f }},
			{{  0.5f,  0.5f, 0.0f }},
			{{ -0.5f,  0.5f, 0.0f }}
		};
		vBuffer = mist::VertexBuffer::Create(verts);

		std::vector<uint32_t> indices = {
			0, 1, 2
		};
		iBuffer = mist::IndexBuffer::Create(indices);

		testShader = mist::Application::Get().GetShaderLibrary()->Load("assets/test.glsl");
	}

	void SceneWindow::OnEditorUpdate() {

	}

	void SceneWindow::OnRender() {
		testShader->Bind();
		vBuffer->Bind();
		iBuffer->Bind();
	}

	void SceneWindow::SceneWindowDraw() {
		mist::RenderCommand::Draw();
	}
}