#include "VulkanRenderAPI.hpp"
#include "renderer/vulkan/VulkanContext.hpp"
#include "data/RenderTypes.hpp"

namespace mist {
	void VulkanRenderAPI::Initialize() {
		VulkanContext& context = VulkanContext::GetContext();
		context.Initialize();
	}
	
	void VulkanRenderAPI::Shutdown() {
		VulkanContext& context = VulkanContext::GetContext();
		context.Cleanup();
	}
	
	void VulkanRenderAPI::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) {
		VulkanContext& context = VulkanContext::GetContext();
		VkViewport viewport = {
			.x = viewport.x = static_cast<float>(x),
			.y = static_cast<float>(y),
			.width = static_cast<float>(width),
			.height = static_cast<float>(height),
			.minDepth = 0.0f,
			.maxDepth = 1.0f
		};
		context.SetViewport(viewport);
		
		VkRect2D scissor = {
			.offset = { 0, 0 },
			.extent = { width, height }
		};
		context.SetScissor(scissor);
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

	void VulkanRenderAPI::UpdateCamera(Camera& camera) {
		CameraData camData;
		camData.u_ViewProjection = camera.GetViewProjectionMatrix();
		camData.u_Transform = glm::mat4();//camera.GetTransform().GetLocalToWorldMatrix();

		VulkanContext& context = VulkanContext::GetContext();
		context.descriptors.UpdateUniformBuffer({ context.GetCurrentFrameIndex(), "CameraData" }, camData);
	}

	void VulkanRenderAPI::BindMeshRenderer(const MeshRenderer& meshRenderer) {
		VulkanContext& context = VulkanContext::GetContext();

		meshRenderer.vBuffer->Bind();
		meshRenderer.iBuffer->Bind();
		vkCmdBindDescriptorSets(context.GetCurrentFrameCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, context.pipeline.GetGraphicsPipelineLayout(meshRenderer.shaderName), 0, 1, &context.descriptors.GetDescriptorSet(meshRenderer), 0, nullptr);
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