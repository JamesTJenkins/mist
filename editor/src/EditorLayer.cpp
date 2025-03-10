#include "EditorLayer.hpp"
#include <imgui.h>
#include <Application.hpp>
#include <renderer/RenderCommand.hpp>

namespace mistEditor {
    EditorLayer::EditorLayer() : Layer("EditorLayer") {
		
    }

    EditorLayer::~EditorLayer() {

    }

    void EditorLayer::OnAttach() {
		// Create editor window
		mist::FramebufferProperties properties;
		properties.attachment = { 
			mist::FramebufferTextureFormat::RGBA8
		};
		properties.width = 1280;
		properties.height = 720;
		framebuffer = mist::Framebuffer::Create(properties);
    }

    void EditorLayer::OnDetach() {

    }

    void EditorLayer::OnUpdate() {
        
    }

    void EditorLayer::OnRender() {
        static bool dockspaceOpen = true;
        static bool opt_fullscreen = true;
        static bool opt_padding = false;
        static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

        // We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
		// because it would be confusing to have two docking targets within each others.
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
		if (opt_fullscreen) {
			const ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImGui::SetNextWindowPos(viewport->WorkPos);
			ImGui::SetNextWindowSize(viewport->WorkSize);
			ImGui::SetNextWindowViewport(viewport->ID);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
			window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
			window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
		} else {
			dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
		}

		// When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background
		// and handle the pass-thru hole, so we ask Begin() to not render a background.
		if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
			window_flags |= ImGuiWindowFlags_NoBackground;

		// Important: note that we proceed even if Begin() returns false (aka window is collapsed).
		// This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
		// all active windows docked into it will lose their parent and become undocked.
		// We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
		// any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
		if (!opt_padding)
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("Mist Dockspace", &dockspaceOpen, window_flags);
		if (!opt_padding)
			ImGui::PopStyleVar();

		if (opt_fullscreen)
			ImGui::PopStyleVar(2);

		// Submit the DockSpace
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable) {
			ImGuiID dockspace_id = ImGui::GetID("Dockspace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
		}

		if (ImGui::BeginMenuBar()) {
			if (ImGui::BeginMenu("File")) {
				if (ImGui::MenuItem("New Scene", "Ctrl+N")) {
					NewScene();
				}
		
				if (ImGui::MenuItem("Open Scene", "Ctrl+O")) {
					OpenScene();
				}
		
				if (ImGui::MenuItem("Save Scene", "Ctrl+S")) {
					SaveSceneAs();
				}
		
				if (ImGui::MenuItem("Exit"))
					mist::Application::Get().Quit();
				ImGui::EndMenu();
			}
		
			ImGui::EndMenuBar();
		}

		// Render other shit here

		ImGui::End();
    }
    
    void EditorLayer::OnEvent(const SDL_Event* e) {}

	void EditorLayer::SetImguiContext() {
		// Sets the imgui context on the exe side otherwise any calls to the imgui context within the exe will cause a crash
		ImGui::SetCurrentContext(mist::Application::Get().GetImguiLayer()->GetContext());
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