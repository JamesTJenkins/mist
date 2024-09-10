#pragma once
#include <vulkan/vulkan.h>
#include <vector>
#include "renderer/FrameBuffer.hpp"

namespace mist {
    class VulkanHelper {
    public:
        static VkFormat FindSupportedFormat(const std::vector<VkFormat>& candidates, VkFormat originalFormat, VkImageTiling tiling, VkFormatFeatureFlags features);
        static VkFormat FindSupportedDepthFormat(VkFormat originalFormat, VkImageTiling tiling, VkFormatFeatureFlags formatFlags);
        static bool IsDepthStencilFormat(VkFormat format);
        static bool IsDepthFormat(FramebufferTextureFormat format);
        static bool IsDepthFormat(VkFormat format);
        static VkFormat GetVkFormat(FramebufferTextureFormat format);
        static FramebufferTextureFormat GetFramebufferTextureFormat(VkFormat format);
    };
}