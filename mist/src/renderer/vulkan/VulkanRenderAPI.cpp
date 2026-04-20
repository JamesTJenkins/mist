#include "VulkanRenderAPI.hpp"
#include "renderer/vulkan/VulkanContext.hpp"
#include "data/RenderTypes.hpp"
#include "Debug.hpp"
#include "Application.hpp"

namespace mist {
	void VulkanRenderAPI::Initialize() {
		VulkanContext& context = VulkanContext::GetContext();
		context.Initialize();
	}
	
	void VulkanRenderAPI::Shutdown() {
		VulkanContext& context = VulkanContext::GetContext();
		context.Cleanup();
	}
	
	void VulkanRenderAPI::SetViewport(uint32_t width, uint32_t height) {
		VulkanContext& context = VulkanContext::GetContext();
		context.SetViewport(width, height);
	}
	
	void VulkanRenderAPI::SetClearColor(glm::vec4& color) {
		clearColor = color;
	}
	
	void VulkanRenderAPI::BeginRenderPass() {
		VulkanContext& context = VulkanContext::GetContext();
		context.BeginRenderPass();
	}

	void VulkanRenderAPI::EndRenderPass() {
		VulkanContext& context = VulkanContext::GetContext();
		context.EndRenderPass();
	}

	void VulkanRenderAPI::UpdateDirectionalLight(DirectionalLight& light) {
		DirectionalLightData lightData;
		lightData.u_LightDir = light.GetTransform().Forward();
		lightData.u_LightColor = light.lightColor;

		VulkanContext& context = VulkanContext::GetContext();
		context.descriptors.UpdateUniformBuffer({ context.GetCurrentFrameIndex(), "DirectionalLightData" }, lightData);
	}

	void VulkanRenderAPI::UpdateCamera(Camera& camera) {
		CameraData camData;
		camData.u_ViewProjectionMatrix = camera.GetViewProjectionMatrix();

		VulkanContext& context = VulkanContext::GetContext();
		context.descriptors.UpdateUniformBuffer({ context.GetCurrentFrameIndex(), "CameraData" }, camData);
	}

	void VulkanRenderAPI::BindMeshRenderer(const MeshRenderer& meshRenderer) {
		VulkanContext& context = VulkanContext::GetContext();

		meshRenderer.vBuffer->Bind();
		meshRenderer.iBuffer->Bind();
		vkCmdBindDescriptorSets(context.GetCurrentFrameCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, context.pipeline.GetGraphicsPipelineLayout(meshRenderer.shaderName), 0, 1, &context.descriptors.GetDescriptorSet(meshRenderer), 0, nullptr);
		
		glm::mat4 matrix = meshRenderer.GetTransform().GetLocalToWorldMatrix();
		Application::Get().GetShaderLibrary()->Get(meshRenderer.shaderName)->SetUniformData("ModelMatrix", sizeof(matrix), &matrix);
	}

	void VulkanRenderAPI::Draw(uint32_t indexCount) {
		VulkanContext& context = VulkanContext::GetContext();
		vkCmdDrawIndexed(context.GetCurrentFrameCommandBuffer(), indexCount, 1, 0, 0, 0);
	}
	
	void VulkanRenderAPI::WaitForIdle() {
		VulkanContext& context = VulkanContext::GetContext();
		vkDeviceWaitIdle(context.GetDevice());
	}
}