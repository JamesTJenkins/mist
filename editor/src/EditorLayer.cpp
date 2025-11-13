#include "EditorLayer.hpp"
#include <Application.hpp>
#include <renderer/RenderCommand.hpp>

namespace mistEditor {
	EditorLayer::EditorLayer() : Layer("EditorLayer") {
		
	}

	EditorLayer::~EditorLayer() {

	}

	void EditorLayer::OnAttach() {
		sceneWindow.Initialize();
	}

	void EditorLayer::OnDetach() {
		sceneWindow.Cleanup();
	}

	void EditorLayer::OnUpdate() {
		sceneWindow.OnEditorUpdate();
	}

	void EditorLayer::OnPreRender() {
		sceneWindow.OnPreRender();
	}

	void EditorLayer::OnRender() {
		sceneWindow.OnRender();
	}
	
	void EditorLayer::OnEvent(const SDL_Event* e) {
		
	}

	void EditorLayer::NewScene() {

	}

	void EditorLayer::OpenScene() {
	
	}

	void EditorLayer::SaveSceneAs() {
	
	}
}