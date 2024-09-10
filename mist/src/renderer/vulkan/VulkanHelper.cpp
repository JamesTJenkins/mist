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

	bool VulkanHelper::IsDepthFormat(FramebufferTextureFormat format) {
		switch (format) {
			case FramebufferTextureFormat::DEPTH16:
			case FramebufferTextureFormat::DEPTH24X8:
			case FramebufferTextureFormat::DEPTH32:
			case FramebufferTextureFormat::DEPTH16_STENCIL8:
			case FramebufferTextureFormat::DEPTH24_STENCIL8:
			case FramebufferTextureFormat::DEPTH32_STENCIL8:
				return true;
			default:
				return false;
		}
	};

	VkFormat VulkanHelper::GetVkFormat(FramebufferTextureFormat format) {
		switch (format) {
		// Color formats
        case FramebufferTextureFormat::RGBA8:				return VK_FORMAT_R8G8B8A8_UNORM;
        case FramebufferTextureFormat::BGRA8:				return VK_FORMAT_B8G8R8A8_UNORM;
        case FramebufferTextureFormat::RGB8:				return VK_FORMAT_R8G8B8_UNORM;
        case FramebufferTextureFormat::BGR8:				return VK_FORMAT_B8G8R8_UNORM;
        case FramebufferTextureFormat::RGBA16F:				return VK_FORMAT_R16G16B16A16_SFLOAT;
        case FramebufferTextureFormat::RGBA32F:				return VK_FORMAT_R32G32B32A32_SFLOAT;
        case FramebufferTextureFormat::RGB565:				return VK_FORMAT_R5G6B5_UNORM_PACK16;
        case FramebufferTextureFormat::RGBA4:				return VK_FORMAT_R4G4B4A4_UNORM_PACK16;
        case FramebufferTextureFormat::RG8:					return VK_FORMAT_R8G8_UNORM;
        case FramebufferTextureFormat::RG16F:				return VK_FORMAT_R16G16_SFLOAT;
        case FramebufferTextureFormat::R32F:				return VK_FORMAT_R32_SFLOAT;
        case FramebufferTextureFormat::R11F_G11F_B10F:		return VK_FORMAT_B10G11R11_UFLOAT_PACK32;
        case FramebufferTextureFormat::RGB9_E5:				return VK_FORMAT_E5B9G9R9_UFLOAT_PACK32;
        case FramebufferTextureFormat::R8:					return VK_FORMAT_R8_UNORM;
        case FramebufferTextureFormat::SR8:					return VK_FORMAT_R8_SRGB;
        case FramebufferTextureFormat::SRGB8_ALPHA8:		return VK_FORMAT_R8G8B8A8_SRGB;
        case FramebufferTextureFormat::SBGRA8:				return VK_FORMAT_B8G8R8A8_SRGB;
        case FramebufferTextureFormat::RGB10_A2:			return VK_FORMAT_A2B10G10R10_UNORM_PACK32;
        case FramebufferTextureFormat::R16:					return VK_FORMAT_R16_UNORM;
        // Compressed color formats
        case FramebufferTextureFormat::BC1_RGB:				return VK_FORMAT_BC1_RGB_UNORM_BLOCK;
        case FramebufferTextureFormat::BC1_RGBA:			return VK_FORMAT_BC1_RGBA_UNORM_BLOCK;
        case FramebufferTextureFormat::BC2:					return VK_FORMAT_BC2_UNORM_BLOCK;
        case FramebufferTextureFormat::BC3:					return VK_FORMAT_BC3_UNORM_BLOCK;
        case FramebufferTextureFormat::BC4:					return VK_FORMAT_BC4_UNORM_BLOCK;
        case FramebufferTextureFormat::BC5:					return VK_FORMAT_BC5_UNORM_BLOCK;
        case FramebufferTextureFormat::BC6H:				return VK_FORMAT_BC6H_UFLOAT_BLOCK;
        case FramebufferTextureFormat::BC7:					return VK_FORMAT_BC7_UNORM_BLOCK;
        case FramebufferTextureFormat::ETC2_RGB:			return VK_FORMAT_ETC2_R8G8B8_UNORM_BLOCK;
        case FramebufferTextureFormat::ETC2_RGBA1:			return VK_FORMAT_ETC2_R8G8B8A1_UNORM_BLOCK;
        case FramebufferTextureFormat::ETC2_RGBA8:			return VK_FORMAT_ETC2_R8G8B8A8_UNORM_BLOCK;
        case FramebufferTextureFormat::EAC_R11:				return VK_FORMAT_EAC_R11_UNORM_BLOCK;
        case FramebufferTextureFormat::EAC_RG11:			return VK_FORMAT_EAC_R11G11_UNORM_BLOCK;
        case FramebufferTextureFormat::ASTC_4x4:			return VK_FORMAT_ASTC_4x4_UNORM_BLOCK;
        case FramebufferTextureFormat::ASTC_5x4:			return VK_FORMAT_ASTC_5x4_UNORM_BLOCK;
        case FramebufferTextureFormat::ASTC_5x5:			return VK_FORMAT_ASTC_5x5_UNORM_BLOCK;
        case FramebufferTextureFormat::ASTC_6x5:			return VK_FORMAT_ASTC_6x5_UNORM_BLOCK;
        case FramebufferTextureFormat::ASTC_6x6:			return VK_FORMAT_ASTC_6x6_UNORM_BLOCK;
        case FramebufferTextureFormat::ASTC_8x5:			return VK_FORMAT_ASTC_8x5_UNORM_BLOCK;
        case FramebufferTextureFormat::ASTC_8x6:			return VK_FORMAT_ASTC_8x6_UNORM_BLOCK;
        case FramebufferTextureFormat::ASTC_8x8:			return VK_FORMAT_ASTC_8x8_UNORM_BLOCK;
        case FramebufferTextureFormat::ASTC_10x5:			return VK_FORMAT_ASTC_10x5_UNORM_BLOCK;
        case FramebufferTextureFormat::ASTC_10x6:			return VK_FORMAT_ASTC_10x6_UNORM_BLOCK;
        case FramebufferTextureFormat::ASTC_10x8:			return VK_FORMAT_ASTC_10x8_UNORM_BLOCK;
        case FramebufferTextureFormat::ASTC_10x10:			return VK_FORMAT_ASTC_10x10_UNORM_BLOCK;
        case FramebufferTextureFormat::ASTC_12x10:			return VK_FORMAT_ASTC_12x10_UNORM_BLOCK;
        case FramebufferTextureFormat::ASTC_12x12:			return VK_FORMAT_ASTC_12x12_UNORM_BLOCK;
        // Depth/Stencil formats
        case FramebufferTextureFormat::DEPTH16:				return VK_FORMAT_D16_UNORM;
        case FramebufferTextureFormat::DEPTH24X8:			return VK_FORMAT_X8_D24_UNORM_PACK32;
        case FramebufferTextureFormat::DEPTH32:				return VK_FORMAT_D32_SFLOAT;
        case FramebufferTextureFormat::DEPTH16_STENCIL8:	return VK_FORMAT_D16_UNORM_S8_UINT;
        case FramebufferTextureFormat::DEPTH24_STENCIL8:	return VK_FORMAT_D24_UNORM_S8_UINT;
        case FramebufferTextureFormat::DEPTH32_STENCIL8:	return VK_FORMAT_D32_SFLOAT_S8_UINT;
        case FramebufferTextureFormat::STENCIL8:			return VK_FORMAT_S8_UINT;
		default:											return VK_FORMAT_UNDEFINED;
		}
	}

	FramebufferTextureFormat VulkanHelper::GetFramebufferTextureFormat(VkFormat format) {
		switch (format) {
        // Color formats
        case VK_FORMAT_R8G8B8A8_UNORM:					return FramebufferTextureFormat::RGBA8;
        case VK_FORMAT_B8G8R8A8_UNORM:					return FramebufferTextureFormat::BGRA8;
        case VK_FORMAT_R8G8B8_UNORM:					return FramebufferTextureFormat::RGB8;
        case VK_FORMAT_B8G8R8_UNORM:					return FramebufferTextureFormat::BGR8;
        case VK_FORMAT_R16G16B16A16_SFLOAT:				return FramebufferTextureFormat::RGBA16F;
        case VK_FORMAT_R32G32B32A32_SFLOAT:				return FramebufferTextureFormat::RGBA32F;
        case VK_FORMAT_R5G6B5_UNORM_PACK16:				return FramebufferTextureFormat::RGB565;
        case VK_FORMAT_R4G4B4A4_UNORM_PACK16:			return FramebufferTextureFormat::RGBA4;
        case VK_FORMAT_R8G8_UNORM:						return FramebufferTextureFormat::RG8;
        case VK_FORMAT_R16G16_SFLOAT:					return FramebufferTextureFormat::RG16F;
        case VK_FORMAT_R32_SFLOAT:						return FramebufferTextureFormat::R32F;
        case VK_FORMAT_B10G11R11_UFLOAT_PACK32:			return FramebufferTextureFormat::R11F_G11F_B10F;
        case VK_FORMAT_E5B9G9R9_UFLOAT_PACK32:			return FramebufferTextureFormat::RGB9_E5;
        case VK_FORMAT_R8_UNORM:						return FramebufferTextureFormat::R8;
        case VK_FORMAT_R8_SRGB:							return FramebufferTextureFormat::SR8;
        case VK_FORMAT_R8G8B8A8_SRGB:					return FramebufferTextureFormat::SRGB8_ALPHA8;
        case VK_FORMAT_B8G8R8A8_SRGB:					return FramebufferTextureFormat::SBGRA8;
        case VK_FORMAT_A2B10G10R10_UNORM_PACK32:		return FramebufferTextureFormat::RGB10_A2;
        case VK_FORMAT_R16_UNORM:						return FramebufferTextureFormat::R16;
        // Compressed color formats
        case VK_FORMAT_BC1_RGB_UNORM_BLOCK:				return FramebufferTextureFormat::BC1_RGB;
        case VK_FORMAT_BC1_RGBA_UNORM_BLOCK:			return FramebufferTextureFormat::BC1_RGBA;
        case VK_FORMAT_BC2_UNORM_BLOCK:					return FramebufferTextureFormat::BC2;
        case VK_FORMAT_BC3_UNORM_BLOCK:					return FramebufferTextureFormat::BC3;
        case VK_FORMAT_BC4_UNORM_BLOCK:					return FramebufferTextureFormat::BC4;
        case VK_FORMAT_BC5_UNORM_BLOCK:					return FramebufferTextureFormat::BC5;
        case VK_FORMAT_BC6H_UFLOAT_BLOCK:				return FramebufferTextureFormat::BC6H;
        case VK_FORMAT_BC7_UNORM_BLOCK:					return FramebufferTextureFormat::BC7;
        case VK_FORMAT_ETC2_R8G8B8_UNORM_BLOCK:			return FramebufferTextureFormat::ETC2_RGB;
        case VK_FORMAT_ETC2_R8G8B8A1_UNORM_BLOCK:		return FramebufferTextureFormat::ETC2_RGBA1;
        case VK_FORMAT_ETC2_R8G8B8A8_UNORM_BLOCK:		return FramebufferTextureFormat::ETC2_RGBA8;
        case VK_FORMAT_EAC_R11_UNORM_BLOCK:				return FramebufferTextureFormat::EAC_R11;
        case VK_FORMAT_EAC_R11G11_UNORM_BLOCK:			return FramebufferTextureFormat::EAC_RG11;
        case VK_FORMAT_ASTC_4x4_UNORM_BLOCK:			return FramebufferTextureFormat::ASTC_4x4;
        case VK_FORMAT_ASTC_5x4_UNORM_BLOCK:			return FramebufferTextureFormat::ASTC_5x4;
        case VK_FORMAT_ASTC_5x5_UNORM_BLOCK:			return FramebufferTextureFormat::ASTC_5x5;
        case VK_FORMAT_ASTC_6x5_UNORM_BLOCK:			return FramebufferTextureFormat::ASTC_6x5;
        case VK_FORMAT_ASTC_6x6_UNORM_BLOCK:			return FramebufferTextureFormat::ASTC_6x6;
        case VK_FORMAT_ASTC_8x5_UNORM_BLOCK:			return FramebufferTextureFormat::ASTC_8x5;
        case VK_FORMAT_ASTC_8x6_UNORM_BLOCK:			return FramebufferTextureFormat::ASTC_8x6;
        case VK_FORMAT_ASTC_8x8_UNORM_BLOCK:			return FramebufferTextureFormat::ASTC_8x8;
        case VK_FORMAT_ASTC_10x5_UNORM_BLOCK:			return FramebufferTextureFormat::ASTC_10x5;
        case VK_FORMAT_ASTC_10x6_UNORM_BLOCK:			return FramebufferTextureFormat::ASTC_10x6;
        case VK_FORMAT_ASTC_10x8_UNORM_BLOCK:			return FramebufferTextureFormat::ASTC_10x8;
        case VK_FORMAT_ASTC_10x10_UNORM_BLOCK:			return FramebufferTextureFormat::ASTC_10x10;
        case VK_FORMAT_ASTC_12x10_UNORM_BLOCK:			return FramebufferTextureFormat::ASTC_12x10;
        case VK_FORMAT_ASTC_12x12_UNORM_BLOCK:			return FramebufferTextureFormat::ASTC_12x12;
        // Depth/Stencil formats
        case VK_FORMAT_D16_UNORM:						return FramebufferTextureFormat::DEPTH16;
        case VK_FORMAT_X8_D24_UNORM_PACK32:				return FramebufferTextureFormat::DEPTH24X8;
        case VK_FORMAT_D32_SFLOAT:						return FramebufferTextureFormat::DEPTH32;
        case VK_FORMAT_D16_UNORM_S8_UINT:				return FramebufferTextureFormat::DEPTH16_STENCIL8;
        case VK_FORMAT_D24_UNORM_S8_UINT:				return FramebufferTextureFormat::DEPTH24_STENCIL8;
        case VK_FORMAT_D32_SFLOAT_S8_UINT:				return FramebufferTextureFormat::DEPTH32_STENCIL8;
        case VK_FORMAT_S8_UINT:							return FramebufferTextureFormat::STENCIL8;
		default:										return FramebufferTextureFormat::None;
		}
	}
}