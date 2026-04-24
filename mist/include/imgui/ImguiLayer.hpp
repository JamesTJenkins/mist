#pragma once
#include "Layer.hpp"
#include <imgui.h>
#include "renderer/Framebuffer.hpp"

namespace mist {
	using ImGuiTextureID = uint32_t;

	class ImguiLayer : public Layer {
	public:
		ImguiLayer(const char* name = "Imgui Layer");
		~ImguiLayer();

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnEvent(const SDL_Event* e) override;

		void Begin();
		void End();
		ImGuiTextureID AddTexture(const Ref<RenderData>& renderData);
		void UpdateTexture(const ImGuiTextureID& id, const Ref<RenderData>& renderData);
		void RemoveTexture(const ImGuiTextureID& id);
		void ImGuiImage(const ImGuiTextureID& id, const ImVec2& imageSize, const ImVec2& uv0, const ImVec2& uv1);
	protected:
		void SetDarkThemeColors();

		Ref<RenderData> renderData;

		// Must fwd declare otherwise would leak vulkan to exe
		class ImguiLayerData;
		ImguiLayerData* layerData;
	};
}