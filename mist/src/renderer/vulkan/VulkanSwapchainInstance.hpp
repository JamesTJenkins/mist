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

	class VulkanSwapchainInstance {
	public:
		VulkanSwapchainInstance(const uint32_t swapchainIndex, const FramebufferProperties& properties);
		~VulkanSwapchainInstance();

		VulkanSwapchainInstance(const VulkanSwapchainInstance& other) = delete;
		VulkanSwapchainInstance& operator=(const VulkanSwapchainInstance& other) = delete;

		const SwapchainSupportDetails QuerySwapchainSupport() const;

		void CreateSwapchain(const FramebufferProperties& properties);
		void CreateRenderPass(const FramebufferProperties& properties);
		void BeginRenderPass(VkCommandBuffer commandBuffer);
		void EndRenderPass(VkCommandBuffer commandBuffer);

		inline const VkSwapchainKHR GetSwapchain() const { return swapchain; }
		inline const Ref<VulkanFramebuffer> GetFrameBuffer() const { return framebuffers[activeFramebuffer]; }
		inline const std::vector<VulkanImage> GetSwapchainImages() const { return framebuffers[activeFramebuffer].get()->GetImages(); }
		inline const VkRenderPass GetRenderPass() const { return renderpass; }
		inline const uint32_t GetSwapchainMinImageCount() const { return swapchainMinImageCount; }
		inline const uint32_t GetSwapchainImageCount() const { return swapchainImageCount; }
	private:
		uint32_t swapchainIndex;
		uint32_t swapchainMinImageCount;
		uint32_t swapchainImageCount;
		uint8_t activeFramebuffer = 0;	// Currently rendered image
		VkSwapchainKHR swapchain;
		std::vector<Ref<VulkanFramebuffer>> framebuffers;
		VkFormat swapchainImageFormat;
		VkExtent2D swapchainExtent;
		VkRenderPass renderpass = VK_NULL_HANDLE;
	};
}