#include "EditorLayer.hpp"
#include <Application.hpp>
#include <renderer/RenderCommand.hpp>

namespace mistEditor {
    EditorLayer::EditorLayer() : Layer("EditorLayer") {
		
    }

    EditorLayer::~EditorLayer() {

    }

    void EditorLayer::OnAttach() {
		// Create editor window
        std::vector<mist::FramebufferTextureProperties> attachments = {
			mist::FramebufferTextureFormat::RGBA8
		};
		mist::FramebufferProperties properties;
        properties.attachment.attachments = attachments.data();
        properties.attachment.attachmentsCount = (uint32_t)attachments.size();
		properties.width = 1280;
		properties.height = 720;
		mist::Framebuffer::Create(properties);
    }

    void EditorLayer::OnDetach() {

    }

    void EditorLayer::OnUpdate() {
        
    }

    void EditorLayer::OnRender() {
		
    }
    
    void EditorLayer::OnEvent(const SDL_Event* e) {
		
	}

    void EditorLayer::NewScene() {
        // TODO: make a scene first howaboutthatbitch
    }

    void EditorLayer::OpenScene() {
        // TODO: make a scene first howaboutthatbitch
    }

    void EditorLayer::SaveSceneAs() {
        // TODO: make a scene first howaboutthatbitch
    }
}