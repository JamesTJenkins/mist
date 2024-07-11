#include "VulkanImage.hpp"
#include "renderer/vulkan/VulkanContext.hpp"
#include "renderer/vulkan/VulkanDebug.hpp"

namespace mist {
    VulkanImage::VulkanImage() {

    }

    VulkanImage::VulkanImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, uint32_t mipLevels) {
        CreateImage(width, height, format, tiling, usage, properties, mipLevels);
    }

    VulkanImage::VulkanImage(VkImage image) : image(image) {
        // This is typically only used by the swapchain and the imageMemory 
        // will already be allocated and bound so wont require to do again
    }

    VulkanImage::~VulkanImage() {
        VulkanContext& context = VulkanContext::GetContext();
        if (image != VK_NULL_HANDLE) {
            vkDestroyImage(context.GetDevice(), image, context.GetAllocationCallbacks());
        }

        if (imageMemory != VK_NULL_HANDLE) {
            vkFreeMemory(context.GetDevice(), imageMemory, context.GetAllocationCallbacks());
        }
    }

    void VulkanImage::CreateImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, uint32_t mipLevels) {
        VkImageCreateInfo info {};
        info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        info.imageType = VK_IMAGE_TYPE_2D;
        info.extent.width = width;
        info.extent.height = height;
        info.extent.depth = 1;
        info.mipLevels = mipLevels;
        info.arrayLayers = 1;
        info.format = format;
        info.tiling = tiling;
        info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        info.usage = usage;
        info.samples = VK_SAMPLE_COUNT_1_BIT;
        info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        VulkanContext& context = VulkanContext::GetContext();

        CheckVkResult(vkCreateImage(context.GetDevice(), &info, context.GetAllocationCallbacks(), &image));

        VkMemoryRequirements memRequirements;
        vkGetImageMemoryRequirements(context.GetDevice(), image, &memRequirements);

        VkMemoryAllocateInfo memInfo {};
        memInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        memInfo.allocationSize = memRequirements.size;
        memInfo.memoryTypeIndex = context.FindMemoryType(memRequirements.memoryTypeBits, properties);

        CheckVkResult(vkAllocateMemory(context.GetDevice(), &memInfo, context.GetAllocationCallbacks(), &imageMemory));

        vkBindImageMemory(context.GetDevice(), image, imageMemory, 0);
    }
}