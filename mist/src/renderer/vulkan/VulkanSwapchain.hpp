#pragma once
#include <vulkan/vulkan.h>
#include <vector>
#include "Core.hpp"
#include "renderer/vulkan/VulkanImage.hpp"
#include "renderer/vulkan/VulkanFramebuffer.hpp"

namespace mist {
	struct SwapchainSupportDetails {
		VkSurfaceCapabilitiesKHR capabilities;
		std::vector<VkSurfaceFormatKHR> formats;
		std::vector<VkPresentModeKHR> presentMode;
	};

	class VulkanSwapchain {
	public:
		VulkanSwapchain();
		~VulkanSwapchain();

		VulkanSwapchain(const VulkanSwapchain& other) = delete;
		VulkanSwapchain& operator=(const VulkanSwapchain& other) = delete;

		const SwapchainSupportDetails QuerySwapchainSupport() const;

		void RecreateSwapchain();
		void CreateSwapchain(FramebufferProperties& properties);
		void CreateRenderPass(const FramebufferProperties& properties);
		void BeginRenderPass(VkCommandBuffer commandBuffer);
		void EndRenderPass(VkCommandBuffer commandBuffer);

		inline const VkSwapchainKHR GetSwapchain() const { return swapchain; }
		inline const uint8_t GetCurrentFrameIndex() const { return activeFramebuffer; }
		inline const Ref<VulkanFramebuffer> GetFrameBuffer() const { return framebuffers[GetCurrentFrameIndex()]; }
		inline const std::vector<VulkanImage> GetSwapchainImages() const { return framebuffers[GetCurrentFrameIndex()].get()->GetImages(); }
		inline const VkRenderPass GetRenderPass() const { return renderpass; }
		inline const uint32_t GetSwapchainMinImageCount() const { return swapchainMinImageCount; }
		inline const uint32_t GetSwapchainImageCount() const { return swapchainImageCount; }
		inline const uint32_t GetSubpassColorAttachmentRefCount() const { return subpassColorAttachmentRefsCount; }
	private:
		uint32_t swapchainMinImageCount;
		uint32_t swapchainImageCount;
		uint8_t activeFramebuffer = 0;	// Currently rendered image
		VkSwapchainKHR swapchain;
		std::vector<Ref<VulkanFramebuffer>> framebuffers;
		VkFormat swapchainImageFormat;
		VkExtent2D swapchainExtent;
		VkRenderPass renderpass = VK_NULL_HANDLE;
		uint32_t subpassColorAttachmentRefsCount;
		FramebufferProperties swapchainProperties;
	};
}