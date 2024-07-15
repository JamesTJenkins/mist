#pragma once
#include <vulkan/vulkan.h>
#include "renderer/FrameBuffer.hpp"
#include "renderer/vulkan/VulkanImage.hpp"

namespace mist {
    class VulkanFrameBuffer : public FrameBuffer {
    public:
        VulkanFrameBuffer();
        VulkanFrameBuffer(const FrameBufferProperties& properties, VkRenderPass renderpass, VulkanImage swapchainImage);
        virtual ~VulkanFrameBuffer();

        VulkanFrameBuffer(const VulkanFrameBuffer& other) = delete;
        VulkanFrameBuffer& operator=(const VulkanFrameBuffer& other) = delete;

        void Create(VkRenderPass renderpass, VulkanImage swapchainImage);
        void Destroy();

        virtual void Resize(uint32_t width, uint32_t height) override;

        virtual uint32_t GetColorAttachmentRenderID(uint32_t index = 0) const override;
        virtual const FrameBufferProperties& GetProperties() const override { return properties; };

        inline const std::vector<VulkanImage> GetImages() const { return attachmentImages; }
    private:
        VkFramebuffer frameBuffer = VK_NULL_HANDLE;
        std::vector<VulkanImage> attachmentImages;

        FrameBufferProperties properties;
        std::vector<FrameBufferTextureProperties> colorAttachmentProperties;
        std::vector<FrameBufferTextureProperties> depthAttachmentProperties;
        std::vector<uint32_t> colorAttachments;
        std::vector<uint32_t> depthAttachment;
    };
}