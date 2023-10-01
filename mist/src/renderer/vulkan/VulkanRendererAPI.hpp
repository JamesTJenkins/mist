#pragma once
#include "renderer/RendererAPI.hpp"
#include <SDL2/SDL_vulkan.h>
#include <vulkan/vulkan.hpp>

namespace mist {
    class VulkanRendererAPI : public RendererAPI {
    public:
        virtual void Initialize() override;
        virtual void Shutdown() override;

        virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;

        virtual void SetClearColor(glm::vec4& color) override;
        virtual void Clear() override;
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