#include "SceneWindow.hpp"
#include <imgui.h>
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
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
		ImGui::Begin("Scene");
		focused = ImGui::IsWindowFocused() && ImGui::IsWindowHovered();
		ImVec2 imguiViewportSize = ImGui::GetContentRegionAvail();
		if (viewportSize != *((glm::vec2*)&imguiViewportSize)) {
			framebuffer->Resize(static_cast<uint32_t>(imguiViewportSize.x), static_cast<uint32_t>(imguiViewportSize.y));
			viewportSize = { imguiViewportSize.x, imguiViewportSize.y };
		}
		ImGui::Image((void*)framebuffer->GetColorAttachmentRenderID(), imguiViewportSize, ImVec2{0, 1}, ImVec2{1, 0});
		
		ImVec2 offset = ImGui::GetWindowPos();
		ImVec2 minRegion = ImGui::GetWindowContentRegionMin();
		ImVec2 maxRegion = ImGui::GetWindowContentRegionMax();
		viewportBounds[0] = { minRegion.x + offset.x, minRegion.y + offset.y };
		viewportBounds[1] = { maxRegion.x + offset.x, maxRegion.y + offset.y };

		SceneWindowDraw();

		ImGui::End();
		ImGui::PopStyleVar();
	}

	void SceneWindow::SceneWindowDraw() {
		mist::RenderCommand::Draw();
	}
}