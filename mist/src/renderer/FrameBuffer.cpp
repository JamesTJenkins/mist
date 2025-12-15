#include "renderer/Framebuffer.hpp"
#include "renderer/vulkan/VulkanContext.hpp"
#include "renderer/vulkan/VulkanHelper.hpp"
#include "Debug.hpp"
#include "Application.hpp"

namespace mist {
	const char* FramebufferTextureFormatToString(const FramebufferTextureFormat format) {
		switch (format) {
			case FramebufferTextureFormat::None:				return "None";
			case FramebufferTextureFormat::RGBA8:				return "RGBA8";
			case FramebufferTextureFormat::BGRA8:				return "BGRA8";
			case FramebufferTextureFormat::RGB8:				return "RGB8";
			case FramebufferTextureFormat::BGR8:				return "BGR8";
			case FramebufferTextureFormat::RGBA16F:				return "RGBA16F";
			case FramebufferTextureFormat::RGBA32F:				return "RGBA32F";
			case FramebufferTextureFormat::RGB565:				return "RGB565";
			case FramebufferTextureFormat::RGBA4:				return "RGBA4";
			case FramebufferTextureFormat::RG8:					return "RG8";
			case FramebufferTextureFormat::RG16F:				return "RG16F";
			case FramebufferTextureFormat::R32F:				return "R32F";
			case FramebufferTextureFormat::R11F_G11F_B10F:		return "R11F_G11F_B10F";
			case FramebufferTextureFormat::RGB9_E5:				return "RGB9_E5";
			case FramebufferTextureFormat::R8:					return "R8";
			case FramebufferTextureFormat::SR8:					return "SR8";
			case FramebufferTextureFormat::SRGB8_ALPHA8:		return "SRGB8_ALPHA8";
			case FramebufferTextureFormat::SBGRA8:				return "SBGRA8";
			case FramebufferTextureFormat::RGB10_A2:			return "RGB10_A2";
			case FramebufferTextureFormat::R16:					return "R16";
			case FramebufferTextureFormat::BC1_RGB:				return "BC1_RGB";
			case FramebufferTextureFormat::BC1_RGBA:			return "BC1_RGBA";
			case FramebufferTextureFormat::BC2:					return "BC2";
			case FramebufferTextureFormat::BC3:					return "BC3";
			case FramebufferTextureFormat::BC4:					return "BC4";
			case FramebufferTextureFormat::BC5:					return "BC5";
			case FramebufferTextureFormat::BC6H:				return "BC6H";
			case FramebufferTextureFormat::BC7:					return "BC7";
			case FramebufferTextureFormat::ETC2_RGB:			return "ETC2_RGB";
			case FramebufferTextureFormat::ETC2_RGBA1:			return "ETC2_RGBA1";
			case FramebufferTextureFormat::ETC2_RGBA8:			return "ETC2_RGBA8";
			case FramebufferTextureFormat::EAC_R11:				return "EAC_R11";
			case FramebufferTextureFormat::EAC_RG11:			return "EAC_RG11";
			case FramebufferTextureFormat::ASTC_4x4:			return "ASTC_4x4";
			case FramebufferTextureFormat::ASTC_5x4:			return "ASTC_5x4";
			case FramebufferTextureFormat::ASTC_5x5:			return "ASTC_5x5";
			case FramebufferTextureFormat::ASTC_6x5:			return "ASTC_6x5";
			case FramebufferTextureFormat::ASTC_6x6:			return "ASTC_6x6";
			case FramebufferTextureFormat::ASTC_8x5:			return "ASTC_8x5";
			case FramebufferTextureFormat::ASTC_8x6:			return "ASTC_8x6";
			case FramebufferTextureFormat::ASTC_8x8:			return "ASTC_8x8";
			case FramebufferTextureFormat::ASTC_10x5:			return "ASTC_10x5";
			case FramebufferTextureFormat::ASTC_10x6:			return "ASTC_10x6";
			case FramebufferTextureFormat::ASTC_10x8:			return "ASTC_10x8";
			case FramebufferTextureFormat::ASTC_10x10:			return "ASTC_10x10";
			case FramebufferTextureFormat::ASTC_12x10:			return "ASTC_12x10";
			case FramebufferTextureFormat::ASTC_12x12:			return "ASTC_12x12";
			case FramebufferTextureFormat::DEPTH16:				return "DEPTH16";
			case FramebufferTextureFormat::DEPTH24X8:			return "DEPTH24X8";
			case FramebufferTextureFormat::DEPTH32:				return "DEPTH32";
			case FramebufferTextureFormat::DEPTH16_STENCIL8:	return "DEPTH16_STENCIL8";
			case FramebufferTextureFormat::DEPTH24_STENCIL8:	return "DEPTH24_STENCIL8";
			case FramebufferTextureFormat::DEPTH32_STENCIL8:	return "DEPTH32_STENCIL8";
			case FramebufferTextureFormat::STENCIL8:			return "STENCIL8";
		}

		return "Missing format";
	}

	void ValidateFramebufferProperties(FramebufferProperties& properties) {
		for (size_t i = 0; i < properties.attachment.attachmentsCount; i++) {
			FramebufferTextureProperties& textureProperties = properties.attachment.attachments[i];
			if (VulkanHelper::IsColorFormatSupported(VulkanHelper::GetVkFormat(textureProperties.textureFormat))) {
				continue;
			}
			if (VulkanHelper::IsDepthStencilFormatSupported(VulkanHelper::GetVkFormat(textureProperties.textureFormat))) {
				continue;
			}
	
			// If format is not supported, choose a fallback format
			if (VulkanHelper::IsDepthStencilFormat(textureProperties.textureFormat)) {
				textureProperties.textureFormat = VulkanHelper::GetFramebufferTextureFormat(VulkanHelper::FindSupportedDepthStencilFormat(VulkanHelper::GetVkFormat(textureProperties.textureFormat), VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT));
			} else {
				textureProperties.textureFormat = VulkanHelper::GetFramebufferTextureFormat(VulkanHelper::FindSupportedColorFormat(VulkanHelper::GetVkFormat(textureProperties.textureFormat), VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT));
			}
	
			MIST_WARN(std::string("Format not supported. Using a fallback format: ") + FramebufferTextureFormatToString(textureProperties.textureFormat));
		}
	}

	void Framebuffer::Create(FramebufferProperties& properties) {
		switch (Application::Get().GetRenderAPI()->GetAPI()) {
		case RenderAPI::API::Vulkan:
		{
			ValidateFramebufferProperties(properties);
			
			VulkanContext& context = VulkanContext::GetContext();
			MIST_INFO("Creating new swapchain for framebuffer");
			context.CreateSwapchain(properties);
			break;
		}
		case RenderAPI::API::None:
			MIST_ASSERT(false, "Running headless");
			break;
		}
	}
}