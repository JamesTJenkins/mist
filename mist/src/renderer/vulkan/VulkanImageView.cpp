#include "VulkanImageView.hpp"
#include "VulkanDebug.hpp"
#include "Log.hpp"
#include "renderer/RenderCommand.hpp"
#include "renderer/vulkan/VulkanContext.hpp"

namespace mist {
    VulkanImageView::VulkanImageView() {

    }

    VulkanImageView::VulkanImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels) {
        CreateImageView(image, format, aspectFlags, mipLevels);
    }

    VulkanImageView::~VulkanImageView() {
        VulkanContext& context = VulkanContext::GetContext();
        vkDestroyImageView(context.GetDevice(), imageView, context.GetAllocationCallbacks());
    }

    void VulkanImageView::CreateImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels) {
        VkImageViewCreateInfo viewInfo{};
        viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.image = image;
        viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        viewInfo.format = format;
        viewInfo.subresourceRange.aspectMask = aspectFlags;
        viewInfo.subresourceRange.baseMipLevel = 0;
        viewInfo.subresourceRange.levelCount = mipLevels;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount = 1;

        VulkanContext& context = VulkanContext::GetContext();
        CheckVkResult(vkCreateImageView(context.GetDevice(), &viewInfo, context.GetAllocationCallbacks(), &imageView));
    }

    VkFormat VulkanImageView::FindSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features) {
        VulkanContext& context = VulkanContext::GetContext();

        for (VkFormat format : candidates) {
            VkFormatProperties properties;
            vkGetPhysicalDeviceFormatProperties(context.GetPhysicalDevice(), format, &properties);

            if (tiling == VK_IMAGE_TILING_LINEAR && (properties.linearTilingFeatures & features) == features) {
                return format;
            } else if (tiling == VK_IMAGE_TILING_OPTIMAL && (properties.optimalTilingFeatures & features) == features) {
                return format;
            }
        }

        MIST_ERROR("Failed to find supported format, defaulting to first candidate");
        return candidates[0];
    }

    VkFormat VulkanImageView::FindDepthFormat() {
        return FindSupportedFormat({ VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT }, VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
    }
}