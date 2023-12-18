#pragma once
#include <optional>
#include "renderer/vulkan/IVulkanRenderAPI.hpp"

namespace mist {
    struct QueueFamilyIndices {
		std::optional<uint32_t> graphicsFamily;
		std::optional<uint32_t> presentFamily;

		bool Valid() { return graphicsFamily.has_value() && presentFamily.has_value(); }
	};

    class VulkanRenderAPI : public IVulkanRenderAPI {
    public:
        // Render API overrides
        virtual void Initialize() override;
        virtual void Shutdown() override;

        virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;

        virtual void SetClearColor(glm::vec4& color) override;
        virtual void Clear() override;

        virtual RenderAPI::API GetAPI() override { return RenderAPI::API::Vulkan; }
        
        const QueueFamilyIndices FindQueueFamilies() const;

        // IVulkanRenderAPI overrides
        inline virtual const VkInstance GetInstance() const override { return instance; }
        inline virtual const VkDevice GetDevice() const override { return device; }
        inline virtual const VkPhysicalDevice GetPhysicalDevice() const override { return physicalDevice; }
        inline virtual const VkAllocationCallbacks* GetAllocationCallbacks() const override { return allocator; }
    private:
        VkAllocationCallbacks* allocator = NULL;
        VkInstance instance = VK_NULL_HANDLE;
        VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
        VkDevice device = VK_NULL_HANDLE;
        VkQueue graphicsQueue = VK_NULL_HANDLE;
        VkQueue presentQueue = VK_NULL_HANDLE;
        VkDescriptorPool descriptorPool = VK_NULL_HANDLE;
        VkDebugReportCallbackEXT debugReport = VK_NULL_HANDLE;
    };
}