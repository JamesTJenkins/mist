#pragma once
#include "renderer/vulkan/IVulkanRenderAPI.hpp"

namespace mist {
    class VulkanRenderAPI : public IVulkanRenderAPI {
    public:
        // Render API overrides
        virtual void Initialize() override;
        virtual void Shutdown() override;

        virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;

        virtual void SetClearColor(glm::vec4& color) override;
        virtual void Clear() override;

        virtual RenderAPI::API GetAPI() override { return RenderAPI::API::Vulkan; }
        
        // IVulkanRenderAPI overrides
        virtual VkInstance GetInstance() override { return instance; }
        virtual VkDevice GetDevice() override { return device; }
        virtual VkPhysicalDevice GetPhysicalDevice() override { return physicalDevice; }
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