#pragma once
#include <vulkan/vulkan.h>

namespace mist {
    struct VulkanImageProperties {
        uint32_t width, height, depth, mipLevels;
        VkFormat format;
        VkImageTiling tiling;
        VkImageUsageFlags usage;
        VkMemoryPropertyFlags properties;
        VkImageAspectFlags viewAspectFlags;
        bool swapchainImage;

        VulkanImageProperties() {}

        VulkanImageProperties(VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImageAspectFlags viewAspectFlags, uint32_t width, uint32_t height, uint32_t mipLevels = 1, uint32_t depth = 1, bool swapchainImage = false) {
            this->width = width;
            this->height = height;
            this->depth = depth;
            this->mipLevels = mipLevels;
            this->format = format;
            this->tiling = tiling;
            this->usage = usage;
            this->properties = properties;
            this->viewAspectFlags = viewAspectFlags;
            this->swapchainImage = swapchainImage;
        }
    };

    class VulkanImage {
    public:
        VulkanImage();
        VulkanImage(const VulkanImageProperties& properties);
        VulkanImage(VkImage image, const VulkanImageProperties& properties);
        ~VulkanImage();

        void CreateImage(const VulkanImageProperties& properties);
        void CreateImageView();
        
        inline const VkImage GetImage() const { return image; }
        inline const VkImageView GetImageView() const { return view; }
        inline const VkDeviceMemory GetImageMemory() const { return imageMemory; }

        VulkanImageProperties imageProperties;
    private:
        VkImage image = VK_NULL_HANDLE;
        VkImageView view = VK_NULL_HANDLE;
        VkDeviceMemory imageMemory = VK_NULL_HANDLE;
    };
}