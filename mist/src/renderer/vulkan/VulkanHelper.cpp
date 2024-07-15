#include "VulkanHelper.hpp"
#include "renderer/vulkan/VulkanContext.hpp"
#include "Debug.hpp"

namespace mist {
    VkFormat VulkanHelper::FindSupportedFormat(const std::vector<VkFormat>& candidates, VkFormat originalFormat, VkImageTiling tiling, VkFormatFeatureFlags features) {
        VulkanContext& context = VulkanContext::GetContext();
        
        VkFormatProperties props;
        vkGetPhysicalDeviceFormatProperties(context.GetPhysicalDevice(), originalFormat, &props);

        // Checks if original was valid
        if ((tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) || (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features)) {
            return originalFormat;
        }

        // Tries alternative formats
        for (VkFormat format : candidates) {
            VkFormatProperties properties;
            vkGetPhysicalDeviceFormatProperties(context.GetPhysicalDevice(), format, &properties);

            if (tiling == VK_IMAGE_TILING_LINEAR && (properties.linearTilingFeatures & features) == features) {
                return format;
            } else if (tiling == VK_IMAGE_TILING_OPTIMAL && (properties.optimalTilingFeatures & features) == features) {
                return format;
            }
        }

        MIST_ERROR("Failed to find supported format, defaulting to original");
        return originalFormat;
    }

    VkFormat VulkanHelper::FindSupportedDepthFormat(VkFormat originalFormat, VkImageTiling tiling, VkFormatFeatureFlags formatFlags) {
        return FindSupportedFormat({ 
            VK_FORMAT_D16_UNORM,
            VK_FORMAT_X8_D24_UNORM_PACK32,
            VK_FORMAT_D32_SFLOAT,
            VK_FORMAT_D16_UNORM_S8_UINT,
            VK_FORMAT_D24_UNORM_S8_UINT,
            VK_FORMAT_D32_SFLOAT_S8_UINT
        }, originalFormat, tiling, formatFlags);
    }

    bool VulkanHelper::IsDepthStencilFormat(VkFormat format) {
        switch (format) {
        case VK_FORMAT_D16_UNORM_S8_UINT:
        case VK_FORMAT_D24_UNORM_S8_UINT:
        case VK_FORMAT_D32_SFLOAT_S8_UINT:
            return true;
        default:
            return false;
        }
    }
    
    bool VulkanHelper::IsDepthFormat(VkFormat format) {
        switch (format) {
        case VK_FORMAT_D16_UNORM:
        case VK_FORMAT_X8_D24_UNORM_PACK32:
        case VK_FORMAT_D32_SFLOAT:
        case VK_FORMAT_D16_UNORM_S8_UINT:
        case VK_FORMAT_D24_UNORM_S8_UINT:
        case VK_FORMAT_D32_SFLOAT_S8_UINT:
            return true;
        default:
            return false;
        }
    }

    bool VulkanHelper::IsDepthFormat(FrameBufferTextureFormat format) {
        switch (format) {
            case FrameBufferTextureFormat::DEPTH16:
            case FrameBufferTextureFormat::DEPTH24X8:
            case FrameBufferTextureFormat::DEPTH32:
            case FrameBufferTextureFormat::DEPTH16STENCIL8:
            case FrameBufferTextureFormat::DEPTH24STENCIL8:
            case FrameBufferTextureFormat::DEPTH32STENCIL8:
                return true;
            default:
                return false;
        }
    };

    VkFormat VulkanHelper::GetVkFormat(FrameBufferTextureFormat format) {
		switch (format) {
		case FrameBufferTextureFormat::RGBA8:			return VK_FORMAT_R8G8B8A8_UNORM;
		case FrameBufferTextureFormat::RINT:			return VK_FORMAT_R32_SINT;
        case FrameBufferTextureFormat::DEPTH16:         return VK_FORMAT_D16_UNORM;
        case FrameBufferTextureFormat::DEPTH24X8:       return VK_FORMAT_X8_D24_UNORM_PACK32;
        case FrameBufferTextureFormat::DEPTH32:         return VK_FORMAT_D32_SFLOAT;
        case FrameBufferTextureFormat::DEPTH16STENCIL8: return VK_FORMAT_D16_UNORM_S8_UINT;
		case FrameBufferTextureFormat::DEPTH24STENCIL8:	return VK_FORMAT_D24_UNORM_S8_UINT;
		case FrameBufferTextureFormat::DEPTH32STENCIL8:	return VK_FORMAT_D32_SFLOAT_S8_UINT;
		default:										return VK_FORMAT_UNDEFINED;
		}
	}
}