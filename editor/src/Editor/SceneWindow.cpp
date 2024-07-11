#include "SceneWindow.hpp"
#include <imgui.h>

namespace mistEditor {
	SceneWindow::SceneWindow() {
		mist::FrameBufferProperties properties;
		properties.attachment = { 
			mist::FrameBufferTextureFormat::RGBA8,
			mist::FrameBufferTextureFormat::RINT,
			mist::FrameBufferTextureFormat::DEPTH24STENCIL8
		};
		properties.width = 1280;
		properties.height = 720;
		frameBuffer = mist::FrameBuffer::Create(properties);
	}

	void SceneWindow::OnEditorUpdate() {

	}

	void SceneWindow::OnRender() {
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
		ImGui::Begin("Scene");
		focused = ImGui::IsWindowFocused() && ImGui::IsWindowHovered();
		ImVec2 imguiViewportSize = ImGui::GetContentRegionAvail();
		if (viewportSize != *((glm::vec2*)&imguiViewportSize)) {
			frameBuffer->Resize(imguiViewportSize.x, imguiViewportSize.y);
			viewportSize = { imguiViewportSize.x, imguiViewportSize.y };
		}
		ImGui::Image((void*)frameBuffer->GetColorAttachmentRenderID(), imguiViewportSize, ImVec2{0, 1}, ImVec2{1, 0});
		
		ImVec2 offset = ImGui::GetWindowPos();
		ImVec2 minRegion = ImGui::GetWindowContentRegionMin();
		ImVec2 maxRegion = ImGui::GetWindowContentRegionMax();
		viewportBounds[0] = { minRegion.x + offset.x, minRegion.y + offset.y };
		viewportBounds[1] = { maxRegion.x + offset.x, maxRegion.y + offset.y };

		ImGui::End();
		ImGui::PopStyleVar();
	}

	void SceneWindow::SceneWindowDraw() {
		// TODO
	}
}