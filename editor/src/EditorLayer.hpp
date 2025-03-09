#pragma once
#include <SDL2/SDL.h>
#include <Layer.hpp>
#include "Editor/SceneWindow.hpp"

namespace mistEditor {
	class EditorLayer : public mist::Layer {
	public:
		EditorLayer();
		virtual ~EditorLayer() override;
	
		EditorLayer(const EditorLayer& other) = delete;
		EditorLayer& operator=(const EditorLayer& other) = delete;

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnUpdate() override;
		virtual void OnRender() override;
		virtual void OnEvent(const SDL_Event* e) override;

		void SetImguiContext();
	private:
		void NewScene();
		void OpenScene();
		void SaveSceneAs();

		mist::Ref<mist::Framebuffer> framebuffer;
		SceneWindow sceneWindow;
	};
}