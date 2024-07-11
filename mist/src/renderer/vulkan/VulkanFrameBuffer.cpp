#include "VulkanFrameBuffer.hpp"
#include "renderer/vulkan/VulkanContext.hpp"
#include "renderer/vulkan/VulkanDebug.hpp"
#include "Debug.hpp"

namespace mist {
	VkFormat GetVkFormat(FrameBufferTextureFormat format) {
		switch (format) {
		case FrameBufferTextureFormat::RGBA8:			return VK_FORMAT_R8G8B8A8_UNORM;
		case FrameBufferTextureFormat::RINT:			return VK_FORMAT_R32_SINT;
		case FrameBufferTextureFormat::DEPTH24STENCIL8:	return VK_FORMAT_D24_UNORM_S8_UINT;
		default:										return VK_FORMAT_UNDEFINED;
		}
	}

	VulkanFrameBuffer::VulkanFrameBuffer() {

	}

	VulkanFrameBuffer::VulkanFrameBuffer(const FrameBufferProperties& properties, uint32_t swapchainInstanceIndex, VulkanImage swapchainImage, VulkanImageView swapchainImageView) : swapchainInstanceIndex(swapchainInstanceIndex), properties(properties) {
		if (properties.width == 0 || properties.height == 0) {
			MIST_WARN("Tried to size framebuffer to {0} {1}", properties.width, properties.height);
			return;
		}

		Create(swapchainImage, swapchainImageView);
	}

	VulkanFrameBuffer::~VulkanFrameBuffer() {
		Destroy();
	}

	void VulkanFrameBuffer::Create(VulkanImage swapchainImage, VulkanImageView swapchainImageView) {
		VulkanContext& context = VulkanContext::GetContext();

		attachmentImages.resize(properties.attachment.attachments.size());
		attachmentImageViews.resize(properties.attachment.attachments.size());

		bool overridedColorBit = false;
		for (size_t i = 0; i < properties.attachment.attachments.size(); ++i) {
			FrameBufferTextureProperties format = properties.attachment.attachments[i];
			bool isDepthFormat = FrameBuffer::IsDepthFormat(format.textureFormat);
			
			if (isDepthFormat) {
				depthAttachmentProperties = format;
			} else {
				colorAttachmentProperties.emplace_back(format);

				if (!overridedColorBit) {
					overridedColorBit = true;

					attachmentImages[i] = swapchainImage;
					attachmentImageViews[i] = swapchainImageView;
					continue;
				}
			}

			attachmentImages[i] = VulkanImage(
				properties.width, 
				properties.height, 
				GetVkFormat(format.textureFormat), 
				VK_IMAGE_TILING_OPTIMAL, 
				isDepthFormat ? VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT : VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
				VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
				1
			);

			attachmentImageViews[i] = VulkanImageView(
				attachmentImages[i].GetImage(),
				GetVkFormat(format.textureFormat),
				isDepthFormat ? VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT : VK_IMAGE_ASPECT_COLOR_BIT,
				1
			);
		}

		std::vector<VkImageView> attachmentViews;
		attachmentViews.resize(attachmentImageViews.size());
		for (size_t i = 0; i < attachmentImageViews.size(); ++i) {
			attachmentViews[i] = attachmentImageViews[i].imageView;
		}

		VkFramebufferCreateInfo info {};
		info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		info.renderPass = context.GetSwapchainInstance(swapchainInstanceIndex).get()->GetRenderPass();
		info.attachmentCount = static_cast<uint32_t>(attachmentViews.size());
		info.pAttachments = attachmentViews.data();
		info.width = properties.width;
		info.height = properties.height;
		info.layers = 1;

		CheckVkResult(vkCreateFramebuffer(context.GetDevice(), &info, context.GetAllocationCallbacks(), &frameBuffer));
	}

	void VulkanFrameBuffer::Destroy() {
		if (frameBuffer == VK_NULL_HANDLE) {
			attachmentImageViews.clear();
			attachmentImages.clear();
			VulkanContext& context = VulkanContext::GetContext();
			vkDestroyFramebuffer(context.GetDevice(), frameBuffer, context.GetAllocationCallbacks());
		}
	}

	void VulkanFrameBuffer::Resize(uint32_t width, uint32_t height) {
		// TODO: might not be needed as this could just handled via swapchain resize
	}

	uint32_t VulkanFrameBuffer::GetColorAttachmentRenderID(uint32_t index) const {
		MIST_ASSERT("Out of bounds", index < colorAttachments.size());
		return colorAttachments[index];
	}
}