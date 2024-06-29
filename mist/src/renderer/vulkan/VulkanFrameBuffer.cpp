#include "renderer/vulkan/VulkanFrameBuffer.hpp"
#include "renderer/vulkan/VulkanContext.hpp"
#include "renderer/vulkan/VulkanDebug.hpp"

namespace mist {
    VulkanFrameBuffer::VulkanFrameBuffer(const FrameBufferProperties& properties) : properties(properties) {
        if (properties.width == 0 || properties.height == 0) {
            MIST_WARN("Tried to size framebuffer to {0} {1}", properties.width, properties.height);
            return;
        }

        for (auto format : properties.attachment.attachments) {
			if (!FrameBuffer::IsDepthFormat(format.textureFormat))
				colorAttachmentProperties.emplace_back(format);
			else
				depthAttachmentProperties = format;
		}

        VulkanContext& context = VulkanContext::GetContext();

        VkFramebufferCreateInfo info {};
        info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        info.renderPass = context.GetRenderPass();
        info.attachmentCount = properties.attachment.attachments.size();
        info.pAttachments = &context.GetSwapchainImageViews()[properties.swapchainId].imageView;
        info.width = properties.width;
        info.height = properties.height;
        info.layers = 1;

        CheckVkResult(vkCreateFramebuffer(context.GetDevice(), &info, context.GetAllocationCallbacks(), &frameBuffer));
    }

    VulkanFrameBuffer::~VulkanFrameBuffer() {
        Destroy();
    }

    void VulkanFrameBuffer::Destroy() {
        if (frameBuffer == VK_NULL_HANDLE) {
            VulkanContext& context = VulkanContext::GetContext();
            vkDestroyFramebuffer(context.GetDevice(), frameBuffer, context.GetAllocationCallbacks());
        }
    }
}