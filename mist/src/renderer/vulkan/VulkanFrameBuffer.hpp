#pragma once
#include <vulkan/vulkan.h>
#include "renderer/Framebuffer.hpp"
#include "renderer/vulkan/VulkanImage.hpp"

namespace mist {
    class VulkanFramebuffer : public Framebuffer {
    public:
        VulkanFramebuffer();
        VulkanFramebuffer(const FramebufferProperties& properties, VkRenderPass renderpass, VulkanImage swapchainImage);
        virtual ~VulkanFramebuffer();

        VulkanFramebuffer(const VulkanFramebuffer& other) = delete;
        VulkanFramebuffer& operator=(const VulkanFramebuffer& other) = delete;

        void Create(VkRenderPass renderpass, VulkanImage swapchainImage);
        void Destroy();

        virtual void Resize(uint32_t width, uint32_t height) override;

        virtual uint32_t GetColorAttachmentRenderID(uint32_t index = 0) const override;
        virtual const FramebufferProperties& GetProperties() const override { return properties; };

        inline const std::vector<VulkanImage> GetImages() const { return attachmentImages; }
        inline const VkFramebuffer GetFramebuffer() const { return framebuffer; }
    private:
        VkFramebuffer framebuffer = VK_NULL_HANDLE;
        std::vector<VulkanImage> attachmentImages;

        FramebufferProperties properties;
        std::vector<FramebufferTextureProperties> colorAttachmentProperties;
        std::vector<FramebufferTextureProperties> depthAttachmentProperties;
        std::vector<uint32_t> colorAttachments;
        std::vector<uint32_t> depthAttachment;
    };
}