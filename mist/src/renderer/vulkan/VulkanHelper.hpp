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
        static bool IsDepthFormat(FrameBufferTextureFormat format);
        static bool IsDepthFormat(VkFormat format);
        static VkFormat GetVkFormat(FrameBufferTextureFormat format);
        static FrameBufferTextureFormat GetFrameBufferTextureFormat(VkFormat format);
    };
}