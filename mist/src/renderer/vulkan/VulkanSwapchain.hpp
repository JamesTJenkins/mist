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

		void ResizeSwapchain(uint32_t newWidth, uint32_t newHeight);
		void RecreateSwapchain();
		void CreateSwapchain(FramebufferProperties& properties);
		void CreateRenderPass(const FramebufferProperties& properties);
		void BeginRenderPass(VkCommandBuffer commandBuffer);
		void EndRenderPass(VkCommandBuffer commandBuffer);
		void CleanupFramebuffers();
		void CleanupRenderPasses();
		void CleanupSwapchain();

		void SetCurrentFrameIndex(const uint8_t newIndex) { activeFramebuffer = newIndex; }

		inline const VkSwapchainKHR& GetSwapchain() { return swapchain; }
		inline const uint8_t GetCurrentFrameIndex() const { return activeFramebuffer; }
		inline const VulkanFramebuffer* GetFrameBuffer() const { return framebuffers[GetCurrentFrameIndex()].get(); }
		inline const VkRenderPass GetRenderPass() const { return renderpass; }
		inline const uint32_t GetSwapchainMinImageCount() const { return swapchainMinImageCount; }
		inline const uint32_t GetSwapchainImageCount() const { return swapchainImageCount; }
		inline const uint32_t GetSubpassColorAttachmentRefCount() const { return subpassColorAttachmentRefsCount; }
	private:
		uint32_t swapchainMinImageCount;
		uint32_t swapchainImageCount;
		uint8_t activeFramebuffer = 0;
		VkSwapchainKHR swapchain = VK_NULL_HANDLE;
		std::vector<Scope<VulkanFramebuffer>> framebuffers;
		VkFormat swapchainImageFormat;
		VkExtent2D swapchainExtent;
		VkRenderPass renderpass = VK_NULL_HANDLE;
		uint32_t subpassColorAttachmentRefsCount;
		FramebufferProperties swapchainProperties;
	};
}