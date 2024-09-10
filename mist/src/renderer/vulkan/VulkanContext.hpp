#pragma once
#include <vulkan/vulkan.h>
#include <optional>
#include <vector>
#include "renderer/vulkan/VulkanDescriptors.hpp"
#include "renderer/vulkan/VulkanCommand.hpp"
#include "renderer/vulkan/VulkanSwapchainInstance.hpp"

namespace mist {
	struct QueueFamilyIndices {
		std::optional<uint32_t> graphicsFamily;
		std::optional<uint32_t> presentFamily;

		bool Valid() { return graphicsFamily.has_value() && presentFamily.has_value(); }
	};

	class VulkanContext {
	public:
		static VulkanContext& GetContext() {
			static VulkanContext instance;
			return instance;
		}

		void CreateInstance();
        void CreateSurface();
        void CreatePhysicalDevice();
        void CreateDevice();
		void CreateSwapchainInstance(const FramebufferProperties& properties);

		const QueueFamilyIndices FindQueueFamilies() const;
		uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags flags);

		inline const VkInstance GetInstance() const { return instance; }
		inline const VkSurfaceKHR GetSurface() const { return surface; }
		inline const VkDevice GetDevice() const { return device; }
		inline const VkPhysicalDevice GetPhysicalDevice() const { return physicalDevice; }
		inline const VkQueue GetGraphicsQueue() const { return graphicsQueue; }
		inline const VkQueue GetPresentQueue() const { return presentQueue; }
		inline const Ref<VulkanSwapchainInstance>& GetSwapchainInstance(uint32_t index = 0) { return swapchainInstances[index]; }
        inline const VkDebugReportCallbackEXT GetDebugCallback() const { return debugReport; }
		inline const VkAllocationCallbacks* GetAllocationCallbacks() const { return allocator; }
		inline const size_t GetSwapchainInstanceCount() const { return swapchainInstances.size(); }

		VulkanDescriptor descriptors;
        VulkanCommand commands;
	private:
        VulkanContext() {}
        ~VulkanContext() {}

        VulkanContext(const VulkanContext&) = delete;
        VulkanContext& operator=(const VulkanContext&) = delete;

		VkAllocationCallbacks* allocator = nullptr;
		VkInstance instance = VK_NULL_HANDLE;
		VkSurfaceKHR surface = VK_NULL_HANDLE;
		VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
		VkDevice device = VK_NULL_HANDLE;
		VkQueue graphicsQueue = VK_NULL_HANDLE;
		VkQueue presentQueue = VK_NULL_HANDLE;
		std::vector<Ref<VulkanSwapchainInstance>> swapchainInstances;
		VkDebugReportCallbackEXT debugReport = VK_NULL_HANDLE;
	};
}