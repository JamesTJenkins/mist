#pragma once
#include "renderer/RenderAPI.hpp"

namespace mist {
	class VulkanRenderAPI : public RenderAPI {
	public:
		// Render API overrides
		virtual void Initialize() override;
		virtual void Shutdown() override;

		virtual void SetClearColor(glm::vec4& color) override;
		virtual glm::vec4 GetClearColor() override { return clearColor; }

		virtual void WaitForIdle() override;
		virtual void BeginFrame() override;
		virtual void EndFrame() override;
		virtual void BeginRenderPass(const uint8_t renderDataID) override;
		virtual void EndRenderPass() override;
		virtual void UpdateDirectionalLight(const uint8_t renderDataID, const DirectionalLight& light) override;
		virtual void UpdateCamera(const uint8_t renderDataID, const Camera& camera) override;
		virtual void BindMeshRenderer(const uint8_t renderDataID, const MeshRenderer& meshRenderer) override;
		virtual void Draw(uint32_t indexCount) override;

		virtual RenderAPI::API GetAPI() override { return RenderAPI::API::Vulkan; }

		virtual void SetVsyncMode(RenderAPI::VSYNC newMode) override { vsync = newMode; }
		virtual RenderAPI::VSYNC GetVsyncMode() override { return vsync; }
	private:
		glm::vec4 clearColor = glm::vec4(0,0,0,1);   // RGBA
		RenderAPI::VSYNC vsync = RenderAPI::VSYNC::Off;
	};
}