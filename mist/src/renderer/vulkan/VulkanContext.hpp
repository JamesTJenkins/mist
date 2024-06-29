#pragma once
#include <vulkan/vulkan.h>
#include <optional>
#include <vector>
#include "renderer/vulkan/VulkanImageView.hpp"
#include "renderer/vulkan/VulkanDescriptors.hpp"
#include "renderer/vulkan/VulkanCommand.hpp"

namespace mist {
	struct QueueFamilyIndices {
		std::optional<uint32_t> graphicsFamily;
		std::optional<uint32_t> presentFamily;

		bool Valid() { return graphicsFamily.has_value() && presentFamily.has_value(); }
	};

	struct SwapchainSupportDetails {
		VkSurfaceCapabilitiesKHR capabilities;
		std::vector<VkSurfaceFormatKHR> formats;
		std::vector<VkPresentModeKHR> presentMode;
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
        void CreateSwapchain();
        void CreateSwapchainImageViews();
        void CreateRenderPass();

		const QueueFamilyIndices FindQueueFamilies() const;
        const SwapchainSupportDetails QuerySwapchainSupport() const;

		inline const VkInstance GetInstance() const { return instance; }
		inline const VkSurfaceKHR GetSurface() const { return surface; }
		inline const VkDevice GetDevice() const { return device; }
		inline const VkPhysicalDevice GetPhysicalDevice() const { return physicalDevice; }
		inline const VkQueue GetGraphicsQueue() const { return graphicsQueue; }
		inline const VkQueue GetPresentQueue() const { return presentQueue; }
        inline const VkSwapchainKHR GetSwapchain() const { return swapchain; }
        inline const std::vector<VkImage> GetSwapchainImages() const { return swapchainImages; }
		inline const std::vector<VulkanImageView> GetSwapchainImageViews() const { return swapchainImageViews; }
		inline const VkRenderPass GetRenderPass() const { return renderpass; }
        inline const VkDebugReportCallbackEXT GetDebugCallback() const { return debugReport; }
		inline const VkAllocationCallbacks* GetAllocationCallbacks() const { return allocator; }

		VulkanDescriptor descriptors;
        VulkanCommand commands;
	private:
        VulkanContext() {}
        ~VulkanContext() {}

        VulkanContext(const VulkanContext&) = delete;
        VulkanContext& operator=(const VulkanContext&) = delete;

		VkAllocationCallbacks* allocator = NULL;
		VkInstance instance = VK_NULL_HANDLE;
		VkSurfaceKHR surface = VK_NULL_HANDLE;
		VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
		VkDevice device = VK_NULL_HANDLE;
		VkQueue graphicsQueue = VK_NULL_HANDLE;
		VkQueue presentQueue = VK_NULL_HANDLE;
		VkDebugReportCallbackEXT debugReport = VK_NULL_HANDLE;
		VkSwapchainKHR swapchain;
		std::vector<VkImage> swapchainImages;
		VkFormat swapchainImageFormat;
		VkExtent2D swapchainExtent;
		std::vector<VulkanImageView> swapchainImageViews;
		VkRenderPass renderpass = VK_NULL_HANDLE;
	};
}