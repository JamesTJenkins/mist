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
			case FrameBufferTextureFormat::DEPTH16_STENCIL8:
			case FrameBufferTextureFormat::DEPTH24_STENCIL8:
			case FrameBufferTextureFormat::DEPTH32_STENCIL8:
				return true;
			default:
				return false;
		}
	};

	VkFormat VulkanHelper::GetVkFormat(FrameBufferTextureFormat format) {
		switch (format) {
		// Color formats
        case FrameBufferTextureFormat::RGBA8:				return VK_FORMAT_R8G8B8A8_UNORM;
        case FrameBufferTextureFormat::BGRA8:				return VK_FORMAT_B8G8R8A8_UNORM;
        case FrameBufferTextureFormat::RGB8:				return VK_FORMAT_R8G8B8_UNORM;
        case FrameBufferTextureFormat::BGR8:				return VK_FORMAT_B8G8R8_UNORM;
        case FrameBufferTextureFormat::RGBA16F:				return VK_FORMAT_R16G16B16A16_SFLOAT;
        case FrameBufferTextureFormat::RGBA32F:				return VK_FORMAT_R32G32B32A32_SFLOAT;
        case FrameBufferTextureFormat::RGB565:				return VK_FORMAT_R5G6B5_UNORM_PACK16;
        case FrameBufferTextureFormat::RGBA4:				return VK_FORMAT_R4G4B4A4_UNORM_PACK16;
        case FrameBufferTextureFormat::RG8:					return VK_FORMAT_R8G8_UNORM;
        case FrameBufferTextureFormat::RG16F:				return VK_FORMAT_R16G16_SFLOAT;
        case FrameBufferTextureFormat::R32F:				return VK_FORMAT_R32_SFLOAT;
        case FrameBufferTextureFormat::R11F_G11F_B10F:		return VK_FORMAT_B10G11R11_UFLOAT_PACK32;
        case FrameBufferTextureFormat::RGB9_E5:				return VK_FORMAT_E5B9G9R9_UFLOAT_PACK32;
        case FrameBufferTextureFormat::R8:					return VK_FORMAT_R8_UNORM;
        case FrameBufferTextureFormat::SR8:					return VK_FORMAT_R8_SRGB;
        case FrameBufferTextureFormat::SRGB8_ALPHA8:		return VK_FORMAT_R8G8B8A8_SRGB;
        case FrameBufferTextureFormat::SBGRA8:				return VK_FORMAT_B8G8R8A8_SRGB;
        case FrameBufferTextureFormat::RGB10_A2:			return VK_FORMAT_A2B10G10R10_UNORM_PACK32;
        case FrameBufferTextureFormat::R16:					return VK_FORMAT_R16_UNORM;
        // Compressed color formats
        case FrameBufferTextureFormat::BC1_RGB:				return VK_FORMAT_BC1_RGB_UNORM_BLOCK;
        case FrameBufferTextureFormat::BC1_RGBA:			return VK_FORMAT_BC1_RGBA_UNORM_BLOCK;
        case FrameBufferTextureFormat::BC2:					return VK_FORMAT_BC2_UNORM_BLOCK;
        case FrameBufferTextureFormat::BC3:					return VK_FORMAT_BC3_UNORM_BLOCK;
        case FrameBufferTextureFormat::BC4:					return VK_FORMAT_BC4_UNORM_BLOCK;
        case FrameBufferTextureFormat::BC5:					return VK_FORMAT_BC5_UNORM_BLOCK;
        case FrameBufferTextureFormat::BC6H:				return VK_FORMAT_BC6H_UFLOAT_BLOCK;
        case FrameBufferTextureFormat::BC7:					return VK_FORMAT_BC7_UNORM_BLOCK;
        case FrameBufferTextureFormat::ETC2_RGB:			return VK_FORMAT_ETC2_R8G8B8_UNORM_BLOCK;
        case FrameBufferTextureFormat::ETC2_RGBA1:			return VK_FORMAT_ETC2_R8G8B8A1_UNORM_BLOCK;
        case FrameBufferTextureFormat::ETC2_RGBA8:			return VK_FORMAT_ETC2_R8G8B8A8_UNORM_BLOCK;
        case FrameBufferTextureFormat::EAC_R11:				return VK_FORMAT_EAC_R11_UNORM_BLOCK;
        case FrameBufferTextureFormat::EAC_RG11:			return VK_FORMAT_EAC_R11G11_UNORM_BLOCK;
        case FrameBufferTextureFormat::ASTC_4x4:			return VK_FORMAT_ASTC_4x4_UNORM_BLOCK;
        case FrameBufferTextureFormat::ASTC_5x4:			return VK_FORMAT_ASTC_5x4_UNORM_BLOCK;
        case FrameBufferTextureFormat::ASTC_5x5:			return VK_FORMAT_ASTC_5x5_UNORM_BLOCK;
        case FrameBufferTextureFormat::ASTC_6x5:			return VK_FORMAT_ASTC_6x5_UNORM_BLOCK;
        case FrameBufferTextureFormat::ASTC_6x6:			return VK_FORMAT_ASTC_6x6_UNORM_BLOCK;
        case FrameBufferTextureFormat::ASTC_8x5:			return VK_FORMAT_ASTC_8x5_UNORM_BLOCK;
        case FrameBufferTextureFormat::ASTC_8x6:			return VK_FORMAT_ASTC_8x6_UNORM_BLOCK;
        case FrameBufferTextureFormat::ASTC_8x8:			return VK_FORMAT_ASTC_8x8_UNORM_BLOCK;
        case FrameBufferTextureFormat::ASTC_10x5:			return VK_FORMAT_ASTC_10x5_UNORM_BLOCK;
        case FrameBufferTextureFormat::ASTC_10x6:			return VK_FORMAT_ASTC_10x6_UNORM_BLOCK;
        case FrameBufferTextureFormat::ASTC_10x8:			return VK_FORMAT_ASTC_10x8_UNORM_BLOCK;
        case FrameBufferTextureFormat::ASTC_10x10:			return VK_FORMAT_ASTC_10x10_UNORM_BLOCK;
        case FrameBufferTextureFormat::ASTC_12x10:			return VK_FORMAT_ASTC_12x10_UNORM_BLOCK;
        case FrameBufferTextureFormat::ASTC_12x12:			return VK_FORMAT_ASTC_12x12_UNORM_BLOCK;
        // Depth/Stencil formats
        case FrameBufferTextureFormat::DEPTH16:				return VK_FORMAT_D16_UNORM;
        case FrameBufferTextureFormat::DEPTH24X8:			return VK_FORMAT_X8_D24_UNORM_PACK32;
        case FrameBufferTextureFormat::DEPTH32:				return VK_FORMAT_D32_SFLOAT;
        case FrameBufferTextureFormat::DEPTH16_STENCIL8:	return VK_FORMAT_D16_UNORM_S8_UINT;
        case FrameBufferTextureFormat::DEPTH24_STENCIL8:	return VK_FORMAT_D24_UNORM_S8_UINT;
        case FrameBufferTextureFormat::DEPTH32_STENCIL8:	return VK_FORMAT_D32_SFLOAT_S8_UINT;
        case FrameBufferTextureFormat::STENCIL8:			return VK_FORMAT_S8_UINT;
		default:											return VK_FORMAT_UNDEFINED;
		}
	}

	FrameBufferTextureFormat VulkanHelper::GetFrameBufferTextureFormat(VkFormat format) {
		switch (format) {
        // Color formats
        case VK_FORMAT_R8G8B8A8_UNORM:					return FrameBufferTextureFormat::RGBA8;
        case VK_FORMAT_B8G8R8A8_UNORM:					return FrameBufferTextureFormat::BGRA8;
        case VK_FORMAT_R8G8B8_UNORM:					return FrameBufferTextureFormat::RGB8;
        case VK_FORMAT_B8G8R8_UNORM:					return FrameBufferTextureFormat::BGR8;
        case VK_FORMAT_R16G16B16A16_SFLOAT:				return FrameBufferTextureFormat::RGBA16F;
        case VK_FORMAT_R32G32B32A32_SFLOAT:				return FrameBufferTextureFormat::RGBA32F;
        case VK_FORMAT_R5G6B5_UNORM_PACK16:				return FrameBufferTextureFormat::RGB565;
        case VK_FORMAT_R4G4B4A4_UNORM_PACK16:			return FrameBufferTextureFormat::RGBA4;
        case VK_FORMAT_R8G8_UNORM:						return FrameBufferTextureFormat::RG8;
        case VK_FORMAT_R16G16_SFLOAT:					return FrameBufferTextureFormat::RG16F;
        case VK_FORMAT_R32_SFLOAT:						return FrameBufferTextureFormat::R32F;
        case VK_FORMAT_B10G11R11_UFLOAT_PACK32:			return FrameBufferTextureFormat::R11F_G11F_B10F;
        case VK_FORMAT_E5B9G9R9_UFLOAT_PACK32:			return FrameBufferTextureFormat::RGB9_E5;
        case VK_FORMAT_R8_UNORM:						return FrameBufferTextureFormat::R8;
        case VK_FORMAT_R8_SRGB:							return FrameBufferTextureFormat::SR8;
        case VK_FORMAT_R8G8B8A8_SRGB:					return FrameBufferTextureFormat::SRGB8_ALPHA8;
        case VK_FORMAT_B8G8R8A8_SRGB:					return FrameBufferTextureFormat::SBGRA8;
        case VK_FORMAT_A2B10G10R10_UNORM_PACK32:		return FrameBufferTextureFormat::RGB10_A2;
        case VK_FORMAT_R16_UNORM:						return FrameBufferTextureFormat::R16;
        // Compressed color formats
        case VK_FORMAT_BC1_RGB_UNORM_BLOCK:				return FrameBufferTextureFormat::BC1_RGB;
        case VK_FORMAT_BC1_RGBA_UNORM_BLOCK:			return FrameBufferTextureFormat::BC1_RGBA;
        case VK_FORMAT_BC2_UNORM_BLOCK:					return FrameBufferTextureFormat::BC2;
        case VK_FORMAT_BC3_UNORM_BLOCK:					return FrameBufferTextureFormat::BC3;
        case VK_FORMAT_BC4_UNORM_BLOCK:					return FrameBufferTextureFormat::BC4;
        case VK_FORMAT_BC5_UNORM_BLOCK:					return FrameBufferTextureFormat::BC5;
        case VK_FORMAT_BC6H_UFLOAT_BLOCK:				return FrameBufferTextureFormat::BC6H;
        case VK_FORMAT_BC7_UNORM_BLOCK:					return FrameBufferTextureFormat::BC7;
        case VK_FORMAT_ETC2_R8G8B8_UNORM_BLOCK:			return FrameBufferTextureFormat::ETC2_RGB;
        case VK_FORMAT_ETC2_R8G8B8A1_UNORM_BLOCK:		return FrameBufferTextureFormat::ETC2_RGBA1;
        case VK_FORMAT_ETC2_R8G8B8A8_UNORM_BLOCK:		return FrameBufferTextureFormat::ETC2_RGBA8;
        case VK_FORMAT_EAC_R11_UNORM_BLOCK:				return FrameBufferTextureFormat::EAC_R11;
        case VK_FORMAT_EAC_R11G11_UNORM_BLOCK:			return FrameBufferTextureFormat::EAC_RG11;
        case VK_FORMAT_ASTC_4x4_UNORM_BLOCK:			return FrameBufferTextureFormat::ASTC_4x4;
        case VK_FORMAT_ASTC_5x4_UNORM_BLOCK:			return FrameBufferTextureFormat::ASTC_5x4;
        case VK_FORMAT_ASTC_5x5_UNORM_BLOCK:			return FrameBufferTextureFormat::ASTC_5x5;
        case VK_FORMAT_ASTC_6x5_UNORM_BLOCK:			return FrameBufferTextureFormat::ASTC_6x5;
        case VK_FORMAT_ASTC_6x6_UNORM_BLOCK:			return FrameBufferTextureFormat::ASTC_6x6;
        case VK_FORMAT_ASTC_8x5_UNORM_BLOCK:			return FrameBufferTextureFormat::ASTC_8x5;
        case VK_FORMAT_ASTC_8x6_UNORM_BLOCK:			return FrameBufferTextureFormat::ASTC_8x6;
        case VK_FORMAT_ASTC_8x8_UNORM_BLOCK:			return FrameBufferTextureFormat::ASTC_8x8;
        case VK_FORMAT_ASTC_10x5_UNORM_BLOCK:			return FrameBufferTextureFormat::ASTC_10x5;
        case VK_FORMAT_ASTC_10x6_UNORM_BLOCK:			return FrameBufferTextureFormat::ASTC_10x6;
        case VK_FORMAT_ASTC_10x8_UNORM_BLOCK:			return FrameBufferTextureFormat::ASTC_10x8;
        case VK_FORMAT_ASTC_10x10_UNORM_BLOCK:			return FrameBufferTextureFormat::ASTC_10x10;
        case VK_FORMAT_ASTC_12x10_UNORM_BLOCK:			return FrameBufferTextureFormat::ASTC_12x10;
        case VK_FORMAT_ASTC_12x12_UNORM_BLOCK:			return FrameBufferTextureFormat::ASTC_12x12;
        // Depth/Stencil formats
        case VK_FORMAT_D16_UNORM:						return FrameBufferTextureFormat::DEPTH16;
        case VK_FORMAT_X8_D24_UNORM_PACK32:				return FrameBufferTextureFormat::DEPTH24X8;
        case VK_FORMAT_D32_SFLOAT:						return FrameBufferTextureFormat::DEPTH32;
        case VK_FORMAT_D16_UNORM_S8_UINT:				return FrameBufferTextureFormat::DEPTH16_STENCIL8;
        case VK_FORMAT_D24_UNORM_S8_UINT:				return FrameBufferTextureFormat::DEPTH24_STENCIL8;
        case VK_FORMAT_D32_SFLOAT_S8_UINT:				return FrameBufferTextureFormat::DEPTH32_STENCIL8;
        case VK_FORMAT_S8_UINT:							return FrameBufferTextureFormat::STENCIL8;
		default:										return FrameBufferTextureFormat::None;
		}
	}
}