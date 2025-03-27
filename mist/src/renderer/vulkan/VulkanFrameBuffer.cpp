#include "VulkanFramebuffer.hpp"
#include "renderer/vulkan/VulkanContext.hpp"
#include "renderer/vulkan/VulkanDebug.hpp"
#include "renderer/vulkan/VulkanHelper.hpp"
#include "Debug.hpp"

namespace mist {
	VulkanFramebuffer::VulkanFramebuffer() {

	}

	VulkanFramebuffer::VulkanFramebuffer(const FramebufferProperties& properties, VkRenderPass renderpass, Ref<VulkanImage> swapchainImage) : properties(properties) {
		if (properties.width == 0 || properties.height == 0) {
			MIST_WARN("Tried to size framebuffer to {0} {1}", properties.width, properties.height);
			return;
		}

		Create(renderpass, swapchainImage);
	}

	VulkanFramebuffer::~VulkanFramebuffer() {
		Cleanup();
	}

	VulkanFramebuffer::VulkanFramebuffer(VulkanFramebuffer&& other) noexcept : 
		framebuffer(other.framebuffer), 
		attachmentImages(std::move(other.attachmentImages)), 
		properties(other.properties),
		colorAttachmentProperties(std::move(other.colorAttachmentProperties)), 
		depthAttachmentProperties(std::move(other.depthAttachmentProperties)), 
		colorAttachments(std::move(other.colorAttachments)), 
		depthAttachment(std::move(other.depthAttachment)) {
		other.framebuffer = VK_NULL_HANDLE;
	}

    VulkanFramebuffer& VulkanFramebuffer::operator=(VulkanFramebuffer&& other) noexcept {
		if (this != &other) {
			Cleanup();
			framebuffer = other.framebuffer;
			attachmentImages = std::move(other.attachmentImages);
			properties = other.properties;
			colorAttachmentProperties = std::move(other.colorAttachmentProperties);
			depthAttachmentProperties = std::move(other.depthAttachmentProperties);
			colorAttachments = std::move(other.colorAttachments);
			depthAttachment = std::move(other.depthAttachment);
		}

		return *this;
	}

	void VulkanFramebuffer::Create(VkRenderPass renderpass, Ref<VulkanImage> swapchainImage) {
		VulkanContext& context = VulkanContext::GetContext();

		attachmentImages.reserve(properties.attachment.attachments.size());

		bool overridedColorBit = false;
		for (size_t i = 0; i < properties.attachment.attachments.size(); ++i) {
			FramebufferTextureProperties format = properties.attachment.attachments[i];
			bool isDepthFormat = VulkanHelper::IsDepthFormat(format.textureFormat);
			
			if (isDepthFormat) {
				depthAttachmentProperties.push_back(format);
			} else {
				colorAttachmentProperties.push_back(format);

				if (!overridedColorBit) {
					overridedColorBit = true;

					attachmentImages.push_back(swapchainImage);
					continue;
				}
			}

			MIST_ASSERT(properties.width < 0 || properties.height > 0, "Tried to create attachment image with size {0, 0}")
			
			VulkanImageProperties props(
				VulkanHelper::GetVkFormat(format.textureFormat),
				VK_IMAGE_TILING_OPTIMAL,
				isDepthFormat ? VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT : VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
				VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
				isDepthFormat ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT,
				properties.width,
				properties.height,
				1,
				1,
				true
			);

			attachmentImages[i] = CreateRef<VulkanImage>(props);
		}

		std::vector<VkImageView> attachmentViews;
		attachmentViews.resize(attachmentImages.size());
		for (size_t i = 0; i < attachmentImages.size(); ++i) {
			attachmentViews[i] = attachmentImages[i]->GetImageView();
		}

		VkFramebufferCreateInfo info {};
		info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		info.renderPass = renderpass;
		info.attachmentCount = static_cast<uint32_t>(attachmentViews.size());
		info.pAttachments = attachmentViews.data();
		info.width = properties.width;
		info.height = properties.height;
		info.layers = 1;

		CheckVkResult(vkCreateFramebuffer(context.GetDevice(), &info, context.GetAllocationCallbacks(), &framebuffer));
	}

	void VulkanFramebuffer::Cleanup() {
		if (framebuffer != VK_NULL_HANDLE) {
			VulkanContext& context = VulkanContext::GetContext();
			vkDestroyFramebuffer(context.GetDevice(), framebuffer, context.GetAllocationCallbacks());
		}

		attachmentImages.clear();
	}

	void VulkanFramebuffer::Resize(uint32_t width, uint32_t height) {
		// TODO: might not be needed as this could just handled via swapchain resize
	}

	uint32_t VulkanFramebuffer::GetColorAttachmentRenderID(uint32_t index) const {
		MIST_ASSERT("Out of bounds", index < colorAttachments.size());
		return colorAttachments[index];
	}
}