#pragma once
#include <vulkan/vulkan.h>
#include "renderer/FrameBuffer.hpp"
#include "renderer/vulkan/VulkanImage.hpp"
#include "renderer/vulkan/VulkanImageView.hpp"

namespace mist {
    class VulkanFrameBuffer : public FrameBuffer {
    public:
        VulkanFrameBuffer();
        VulkanFrameBuffer(const FrameBufferProperties& properties, uint32_t swapchainInstanceIndex, VulkanImage swapchainImage, VulkanImageView swapchainImageView);
        virtual ~VulkanFrameBuffer();

        VulkanFrameBuffer(const VulkanFrameBuffer& other) = delete;
        VulkanFrameBuffer& operator=(const VulkanFrameBuffer& other) = delete;

        void Create(VulkanImage swapchainImage, VulkanImageView swapchainImageView);
        void Destroy();

        virtual void Resize(uint32_t width, uint32_t height) override;

        virtual uint32_t GetColorAttachmentRenderID(uint32_t index = 0) const override;
        virtual const FrameBufferProperties& GetProperties() const override { return properties; };

        inline const std::vector<VulkanImage> GetImages() const { return attachmentImages; }
        inline const std::vector<VulkanImageView> GetImageViews() const { return attachmentImageViews; }
    private:
        uint32_t swapchainInstanceIndex;

        VkFramebuffer frameBuffer;
        std::vector<VulkanImage> attachmentImages;
        std::vector<VulkanImageView> attachmentImageViews;

        FrameBufferProperties properties;
        std::vector<FrameBufferTextureProperties> colorAttachmentProperties;
        FrameBufferTextureProperties depthAttachmentProperties = FrameBufferTextureFormat::None;
        std::vector<uint32_t> colorAttachments;
        uint32_t depthAttachment = 0;
    };
}