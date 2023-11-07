#pragma once
#include <vulkan/vulkan.h>
#include "renderer/RenderAPI.hpp"

namespace mist {
    class IVulkanRenderAPI : public RenderAPI {
        virtual VkInstance GetInstance() = 0;
        virtual VkDevice GetDevice() = 0;
        virtual VkPhysicalDevice GetPhysicalDevice() = 0;
    };
}