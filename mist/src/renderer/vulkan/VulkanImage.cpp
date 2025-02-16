#include "VulkanImage.hpp"
#include "renderer/vulkan/VulkanContext.hpp"
#include "renderer/vulkan/VulkanDebug.hpp"
#include "renderer/vulkan/VulkanHelper.hpp"

namespace mist {
    VulkanImage::VulkanImage() {

    }

    VulkanImage::VulkanImage(const VulkanImageProperties& properties) {
        CreateImage(properties);
        CreateImageView();
    }

    VulkanImage::VulkanImage(VkImage image, const VulkanImageProperties& properties) : imageProperties(properties), image(image) {
        // This is typically only used by the swapchain and the imageMemory 
        // will already be allocated and bound so wont require to do again
    }

    VulkanImage::~VulkanImage() {
        if (!imageProperties.swapchainImage) {
            VulkanContext& context = VulkanContext::GetContext();
            if (view != VK_NULL_HANDLE) {
                vkDestroyImageView(context.GetDevice(), view, context.GetAllocationCallbacks());
            }
    
            if (imageMemory != VK_NULL_HANDLE) {
                vkFreeMemory(context.GetDevice(), imageMemory, context.GetAllocationCallbacks());
            }
    
            if (image != VK_NULL_HANDLE) {
                vkDestroyImage(context.GetDevice(), image, context.GetAllocationCallbacks());
            }
        }
    }

    void VulkanImage::CreateImage(const VulkanImageProperties& properties) {
        imageProperties = properties;

        VkImageCreateInfo info {};
        info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        info.imageType = VK_IMAGE_TYPE_2D;
        info.extent.width = imageProperties.width;
        info.extent.height = imageProperties.height;
        info.extent.depth = imageProperties.depth;
        info.mipLevels = imageProperties.mipLevels;
        info.arrayLayers = 1;
        info.format = imageProperties.format;
        info.tiling = imageProperties.tiling;
        info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        info.usage = imageProperties.usage;
        info.samples = VK_SAMPLE_COUNT_1_BIT;
        info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        info.queueFamilyIndexCount = 0;
        info.pQueueFamilyIndices = nullptr;
        info.flags = 0;
        info.pNext = nullptr;

        VulkanContext& context = VulkanContext::GetContext();
        CheckVkResult(vkCreateImage(context.GetDevice(), &info, context.GetAllocationCallbacks(), &image));

        VkMemoryRequirements memRequirements;
        vkGetImageMemoryRequirements(context.GetDevice(), image, &memRequirements);

        VkMemoryAllocateInfo memInfo {};
        memInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        memInfo.allocationSize = memRequirements.size;
        memInfo.memoryTypeIndex = context.FindMemoryType(memRequirements.memoryTypeBits, properties.properties);

        CheckVkResult(vkAllocateMemory(context.GetDevice(), &memInfo, context.GetAllocationCallbacks(), &imageMemory));

        CheckVkResult(vkBindImageMemory(context.GetDevice(), image, imageMemory, 0));
    }

    void VulkanImage::CreateImageView() {
        VkImageViewCreateInfo viewInfo{};
        viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.image = image;
        viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        viewInfo.format = imageProperties.format;
        viewInfo.subresourceRange.aspectMask = imageProperties.viewAspectFlags;
        viewInfo.subresourceRange.baseMipLevel = 0;
        viewInfo.subresourceRange.levelCount = imageProperties.mipLevels;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount = 1;

        VulkanContext& context = VulkanContext::GetContext();
        CheckVkResult(vkCreateImageView(context.GetDevice(), &viewInfo, context.GetAllocationCallbacks(), &view));
    }
}