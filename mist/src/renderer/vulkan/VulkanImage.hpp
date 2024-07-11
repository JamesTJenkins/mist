#pragma once
#include <vulkan/vulkan.h>

namespace mist {
    class VulkanImage {
    public:
        VulkanImage();
        VulkanImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, uint32_t mipLevels);
        VulkanImage(VkImage image);
        ~VulkanImage();

        void CreateImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, uint32_t mipLevels);
        
        inline const VkImage GetImage() const { return image; }
        inline const VkDeviceMemory GetImageMemory() const { return imageMemory; }
    private:
        VkImage image = VK_NULL_HANDLE;
        VkDeviceMemory imageMemory = VK_NULL_HANDLE;
    };
}