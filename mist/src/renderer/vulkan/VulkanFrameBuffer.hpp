#pragma once
#include "renderer/FrameBuffer.hpp"
#include <Debug.hpp>
#include <vulkan/vulkan.h>

namespace mist {
    class VulkanFrameBuffer : public FrameBuffer {
    public:
        VulkanFrameBuffer(const FrameBufferProperties& properties);
        virtual ~VulkanFrameBuffer();

        VulkanFrameBuffer(const VulkanFrameBuffer& other) = delete;
        VulkanFrameBuffer& operator=(const VulkanFrameBuffer& other) = delete;

        VulkanFrameBuffer(VulkanFrameBuffer&& other) noexcept = default;
        VulkanFrameBuffer& operator=(VulkanFrameBuffer&& other) noexcept = default;

        void Destroy();

        virtual uint32_t GetColorAttachmentRenderID(uint32_t index = 0) const override { MIST_ASSERT("Out of bounds", index < colorAttachments.size()); return colorAttachments[index]; };
        virtual const FrameBufferProperties& GetProperties() const override { return properties; };
    private:
        VkFramebuffer frameBuffer = VK_NULL_HANDLE;
        FrameBufferProperties properties;
        std::vector<FrameBufferTextureProperties> colorAttachmentProperties;
        FrameBufferTextureProperties depthAttachmentProperties = FrameBufferTextureFormat::None;
        std::vector<uint32_t> colorAttachments;
        uint32_t depthAttachment = 0;
    };
}