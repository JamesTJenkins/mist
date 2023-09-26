#pragma once
#include "renderer/RendererAPI.hpp"
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
        VkInstance instance;
        VkPhysicalDevice physicalDevice;
        VkAllocationCallbacks* allocator = NULL;
        VkDebugReportCallbackEXT debugReport;
    };
}