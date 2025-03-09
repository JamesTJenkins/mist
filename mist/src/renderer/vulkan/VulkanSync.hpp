#pragma once
#include <vulkan/vulkan.h>
#include <vector>

namespace mist {
	class VulkanSync {
	public:
		VulkanSync();
		~VulkanSync();

		VulkanSync(const VulkanSync& other) = delete;
		VulkanSync& operator=(const VulkanSync& other) = delete;

		void CreateSyncObjects();
		void Cleanup();

		const int MAX_FRAMES_IN_FLIGHT = 2;
		std::vector<VkSemaphore> imageAvailableSemaphores;
		std::vector<VkSemaphore> renderFinishedSemaphores;
		std::vector<VkFence> inFlightFences;
	};
}