#pragma once
#include "renderer/RenderAPI.hpp"
#include <vulkan/vulkan.h>
#include <optional>
#include <vector>

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

    class VulkanRenderAPI : public RenderAPI {
    public:
        // Render API overrides
        virtual void Initialize() override;
        virtual void Shutdown() override;

        virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;

        virtual void SetClearColor(glm::vec4& color) override;
        virtual void Clear() override;

        virtual RenderAPI::API GetAPI() override { return RenderAPI::API::Vulkan; }
        
        const QueueFamilyIndices FindQueueFamilies() const;
        const SwapchainSupportDetails QuerySwapchainSupport() const;

        void CreateInstance();
        void CreateSurface();
        void CreateSwapchain();

        inline const VkInstance GetInstance() const { return instance; }
        inline const VkSurfaceKHR GetSurface() const { return surface; }
        inline const VkDevice GetDevice() const { return device; }
        inline const VkPhysicalDevice GetPhysicalDevice() const { return physicalDevice; }
        inline const VkQueue GetGraphicsQueue() const { return graphicsQueue; }
        inline const VkQueue GetPresentQueue() const { return presentQueue; }
        inline const VkAllocationCallbacks* GetAllocationCallbacks() const { return allocator; }
    private:
        VkAllocationCallbacks* allocator = NULL;
        VkInstance instance = VK_NULL_HANDLE;
        VkSurfaceKHR surface = VK_NULL_HANDLE;
        VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
        VkDevice device = VK_NULL_HANDLE;
        VkQueue graphicsQueue = VK_NULL_HANDLE;
        VkQueue presentQueue = VK_NULL_HANDLE;
        VkDescriptorPool descriptorPool = VK_NULL_HANDLE;
        VkDebugReportCallbackEXT debugReport = VK_NULL_HANDLE;
        VkSwapchainKHR swapchain;
        std::vector<VkImage> swapchainImages;
        VkFormat swapchainImageFormat;
        VkExtent2D swapchainExtent;
    };
}