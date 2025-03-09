#include "VulkanSync.hpp"
#include "VulkanContext.hpp"
#include "VulkanDebug.hpp"

namespace mist {
	VulkanSync::VulkanSync() {}

	VulkanSync::~VulkanSync() {
		Cleanup();
	}

	void VulkanSync::CreateSyncObjects() {
		imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
		renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
		inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);
	
		VkSemaphoreCreateInfo semaphoreInfo{};
		semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		VkFenceCreateInfo fenceInfo{};
		fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		VulkanContext& context = VulkanContext::GetContext();
		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
			CheckVkResult(vkCreateSemaphore(context.GetDevice(), &semaphoreInfo, context.GetAllocationCallbacks(), &imageAvailableSemaphores[i]));
			CheckVkResult(vkCreateSemaphore(context.GetDevice(), &semaphoreInfo, context.GetAllocationCallbacks(), &renderFinishedSemaphores[i]));
			CheckVkResult(vkCreateFence(context.GetDevice(), &fenceInfo, context.GetAllocationCallbacks(), &inFlightFences[i]));
		}
	}

	void VulkanSync::Cleanup() {
		VulkanContext& context = VulkanContext::GetContext();
		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
			vkDestroySemaphore(context.GetDevice(), imageAvailableSemaphores[i], context.GetAllocationCallbacks());
			vkDestroySemaphore(context.GetDevice(), renderFinishedSemaphores[i], context.GetAllocationCallbacks());
			vkDestroyFence(context.GetDevice(), inFlightFences[i], context.GetAllocationCallbacks());
		}
	}
}