#include "VulkanRenderAPI.hpp"
#include "renderer/vulkan/VulkanContext.hpp"
#include <Log.hpp>
#include "renderer/vulkan/VulkanDebug.hpp"

namespace mist {
	void VulkanRenderAPI::Initialize() {
		VulkanContext& context = VulkanContext::GetContext();

		glm::vec4 initialClearColor = glm::vec4(1.0f, 0.0f, 1.0f, 1.0f); // RGBA: Pink
		SetClearColor(initialClearColor);
		context.CreateInstance();
		context.CreateSurface();
		context.CreatePhysicalDevice();
		context.CreateDevice();
		context.commands.CreateCommandPool(); // TODO: May need to do something more at some point but single pool will do for now
		context.commands.AllocateRenderBuffers();
		context.sync.CreateSyncObjects();
	}

	void VulkanRenderAPI::Shutdown() {
		VulkanContext& context = VulkanContext::GetContext();

		context.descriptors.Cleanup();
		context.descriptors.ClearPool();
		context.pipeline.Cleanup();

		vkDestroyDevice(context.GetDevice(), context.GetAllocationCallbacks());
		vkDestroySurfaceKHR(context.GetInstance(), context.GetSurface(), context.GetAllocationCallbacks());
#ifdef DEBUG
		auto vkDestroyDebugReportCallbackEXT = (PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr(context.GetInstance(), "vkDestroyDebugReportCallbackEXT");
		vkDestroyDebugReportCallbackEXT(context.GetInstance(), context.GetDebugCallback(), context.GetAllocationCallbacks());
#endif
		vkDestroyInstance(context.GetInstance(), context.GetAllocationCallbacks());
	}

	void VulkanRenderAPI::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) {
		
	}

	void VulkanRenderAPI::SetClearColor(glm::vec4& color) {
		clearColor = color;
	}

	void VulkanRenderAPI::Clear() {
		// Does nothing since handles by renderpasses
	}

	void VulkanRenderAPI::BeginRenderPass() {
		VulkanContext& context = VulkanContext::GetContext();
		uint8_t currentFrame = context.GetSwapchain()->GetCurrentFrameIndex();
		
		// Check GPU finished and only wait if not
		VkResult fenceStatus = vkGetFenceStatus(context.GetDevice(), context.sync.inFlightFences[currentFrame]);
		if (fenceStatus != VK_SUCCESS) {
			CheckVkResult(vkWaitForFences(context.GetDevice(), 1, &context.sync.inFlightFences[currentFrame], VK_TRUE, UINT64_MAX));
		}
		CheckVkResult(vkResetFences(context.GetDevice(), 1, &context.sync.inFlightFences[currentFrame]));

		context.commands.ResetCommandBuffer(context.commands.GetRenderBuffer(currentFrame));
		context.commands.BeginCommandBuffer(context.commands.GetRenderBuffer(currentFrame));
		context.GetSwapchain()->BeginRenderPass(context.commands.GetRenderBuffer(currentFrame));
	}

	void VulkanRenderAPI::EndRenderPass() {
		VulkanContext& context = VulkanContext::GetContext();
		uint8_t currentFrame = context.GetSwapchain()->GetCurrentFrameIndex();
		context.GetSwapchain()->EndRenderPass(context.commands.GetRenderBuffer(currentFrame));
		context.commands.EndCommandBuffer(context.commands.GetRenderBuffer(currentFrame));
	}

	void VulkanRenderAPI::Draw() {
		VulkanContext& context = VulkanContext::GetContext();
		uint8_t currentFrame = context.GetSwapchain()->GetCurrentFrameIndex();

		uint32_t imageIndex;
		VkResult result = vkAcquireNextImageKHR(context.GetDevice(), context.GetSwapchain()->GetSwapchain(), UINT64_MAX, context.sync.imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);

		if (result == VK_ERROR_OUT_OF_DATE_KHR) {
			context.GetSwapchain()->RecreateSwapchain();
			return;
		} else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
			CheckVkResult(result);
		}

		if (context.sync.imageAvailableSemaphores[currentFrame] != VK_NULL_HANDLE) {
			VkSubmitInfo submitInfo{};
			submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
			
			VkSemaphore waitSemaphores[] = {context.sync.imageAvailableSemaphores[currentFrame]};
			VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
			submitInfo.waitSemaphoreCount = 1;
			submitInfo.pWaitSemaphores = waitSemaphores;
			submitInfo.pWaitDstStageMask = waitStages;
			submitInfo.commandBufferCount = 1;
			submitInfo.pCommandBuffers = &context.commands.GetRenderBuffer(currentFrame);
			VkSemaphore signalSemaphores[] = {context.sync.renderFinishedSemaphores[currentFrame]};
			submitInfo.signalSemaphoreCount = 1;
			submitInfo.pSignalSemaphores = signalSemaphores;

			CheckVkResult(vkQueueSubmit(context.GetGraphicsQueue(), 1, &submitInfo, context.sync.inFlightFences[currentFrame]));

			VkPresentInfoKHR presentInfo{};
			presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
			presentInfo.waitSemaphoreCount = 1;
			presentInfo.pWaitSemaphores = signalSemaphores;
			presentInfo.swapchainCount = 1;
			presentInfo.pSwapchains = &context.GetSwapchain()->GetSwapchain();
			presentInfo.pImageIndices = &imageIndex;

			CheckVkResult(vkQueuePresentKHR(context.GetPresentQueue(), &presentInfo));

			context.GetSwapchain()->SetCurrentFrameIndex((currentFrame + 1) % context.sync.MAX_FRAMES_IN_FLIGHT);
		}
	}
}