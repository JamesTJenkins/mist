#pragma once
#include <vulkan/vulkan.h>
#include "renderer/RenderAPI.hpp"

namespace mist {
    class IVulkanRenderAPI : public RenderAPI {
        inline virtual const VkInstance GetInstance() const = 0;
        inline virtual const VkDevice GetDevice() const = 0;
        inline virtual const VkPhysicalDevice GetPhysicalDevice() const = 0;
        inline virtual const VkAllocationCallbacks* GetAllocationCallbacks() const = 0;
    };
}