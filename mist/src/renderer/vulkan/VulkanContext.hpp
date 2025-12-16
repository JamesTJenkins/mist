#pragma once
#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>
#include <optional>
#include <vector>
#include "renderer/Framebuffer.hpp"
#include "renderer/vulkan/VulkanPipeline.hpp"
#include "renderer/vulkan/VulkanDescriptors.hpp"

namespace mist {
	struct QueueFamilyIndices {
		std::optional<uint32_t> graphicsFamily;
		std::optional<uint32_t> presentFamily;

		bool Valid() { return graphicsFamily.has_value() && presentFamily.has_value(); }
	};

	struct FramebufferAttachment {
		VkImage image;
		VkImageView view;
		VmaAllocation imageAlloc;

		void Cleanup();
	};

	class VulkanContext {
	public:
		static VulkanContext& GetContext() {
			static VulkanContext instance;
			return instance;
		}

		void Initialize();
        void Cleanup();

		const QueueFamilyIndices FindQueueFamilies() const;
		uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags flags);

		void BeginSingleTimeCommands();
		void EndSingleTimeCommands();
		
		void CreateSwapchain(FramebufferProperties& properties);
		void RecreateSwapchain();
		void BeginRenderPass();
		void EndRenderPass();

		void SetViewport(VkViewport newViewport) { viewport = newViewport; }
		void SetScissor(VkRect2D newScissor) { scissor = newScissor; }

		inline const VkInstance GetInstance() const { return instance; }
		inline const VkSurfaceKHR GetSurface() const { return surface; }
		inline const VkDevice GetDevice() const { return device; }
		inline const VkPhysicalDevice GetPhysicalDevice() const { return physicalDevice; }
		inline const VkQueue GetGraphicsQueue() const { return graphicsQueue; }
		inline const VkQueue GetPresentQueue() const { return presentQueue; }
        inline const VkDebugUtilsMessengerEXT GetDebugMessenger() const { return debugMessenger; }
        inline const VmaAllocator GetAllocator() const { return allocator; }
		inline const VkRenderPass GetRenderPass() const { return renderPass; }
		inline const VkSwapchainKHR GetSwapchain() const { return swapchain; }
		inline const uint32_t GetColorAttachmentCount() const { return colorAttachmentCount; }
		inline const uint32_t GetCurrentFrameIndex() const { return currentFrame; }
		inline const VkCommandBuffer GetCommandBuffer(uint32_t index) const { return commandBuffers[index]; }
		inline const VkCommandBuffer GetCurrentFrameCommandBuffer() const { return commandBuffers[currentFrame]; }
		inline const VkCommandBuffer GetTempCommandBuffer() const { return tempCommandBuffer; }
		inline const VkAllocationCallbacks* GetAllocationCallbacks() const { return allocationCallbacks; }

		VulkanPipeline pipeline;
		VulkanDescriptor descriptors;

		const int MAX_FRAMES_IN_FLIGHT = 3;
	private:
        VulkanContext() {}
        ~VulkanContext() {}

        VulkanContext(const VulkanContext&) = delete;
        VulkanContext& operator=(const VulkanContext&) = delete;

        void CreateInstance();
        void CreateSurface();
        void CreatePhysicalDevice();
        void CreateDevice();
        void CreateAllocator();
		void CreateSemaphores();
		void CreateFences();
		void CreateCommandPool();
		void AllocateCommandBuffers();
		void CreateDescriptorPool();

		uint32_t currentFrame = 0;

		VkInstance instance = VK_NULL_HANDLE;
		VkSurfaceKHR surface = VK_NULL_HANDLE;
		VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
		VkDevice device = VK_NULL_HANDLE;
		VkQueue graphicsQueue = VK_NULL_HANDLE;
		VkQueue presentQueue = VK_NULL_HANDLE;
		VkAllocationCallbacks* allocationCallbacks = nullptr;
		VkDebugUtilsMessengerEXT debugMessenger = VK_NULL_HANDLE;
        VmaAllocator allocator = nullptr;

		VkSwapchainKHR swapchain = VK_NULL_HANDLE;
		FramebufferProperties framebufferProperties;
		VkExtent2D extent;
		VkViewport viewport;
		VkRect2D scissor;
		std::vector<VkImage> swapchainImages;
		std::vector<VkImageView> swapchainImageViews;
		VkRenderPass renderPass = VK_NULL_HANDLE;
		uint32_t colorAttachmentCount = 0;
		std::vector<FramebufferAttachment> additionalFramebufferAttachments;
		std::vector<VkFramebuffer> framebuffers;
		std::vector<VkSemaphore> imageAvailableSemaphores;
		std::vector<VkSemaphore> renderFinishedSemaphores;
		std::vector<VkFence> inFlightFences;

		VkCommandPool commandPool = VK_NULL_HANDLE;
		std::vector<VkCommandBuffer> commandBuffers;
		VkCommandBuffer tempCommandBuffer = VK_NULL_HANDLE;
		VkFence tempCommandBufferFence = VK_NULL_HANDLE;
	};
}