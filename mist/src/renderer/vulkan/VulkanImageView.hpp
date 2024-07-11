#pragma once
#include <vulkan/vulkan.h>
#include <vector>

namespace mist {
    class VulkanImageView {
    public:
        VulkanImageView();
        VulkanImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels);
        ~VulkanImageView();
        
        void CreateImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels);

        static VkFormat FindSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
        static VkFormat FindDepthFormat(); 

        VkImageView imageView = VK_NULL_HANDLE;
    };
}