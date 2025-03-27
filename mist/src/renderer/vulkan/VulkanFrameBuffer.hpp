#pragma once
#include <vulkan/vulkan.h>
#include "renderer/Framebuffer.hpp"
#include "renderer/vulkan/VulkanImage.hpp"

namespace mist {
    class VulkanFramebuffer : public Framebuffer {
    public:
        VulkanFramebuffer();
        VulkanFramebuffer(const FramebufferProperties& properties, VkRenderPass renderpass, Ref<VulkanImage> swapchainImage);
        ~VulkanFramebuffer();

        // Copy (deleted as shouldnt ever be copied)
        VulkanFramebuffer(const VulkanFramebuffer& other) = delete;
        // Copy assignment (deleted as shouldnt ever be copied)
        VulkanFramebuffer& operator=(const VulkanFramebuffer& other) = delete;
        // Move constructor
        VulkanFramebuffer(VulkanFramebuffer&& other) noexcept;
        // Move assignment
        VulkanFramebuffer& operator=(VulkanFramebuffer&& other) noexcept;

        void Create(VkRenderPass renderpass, Ref<VulkanImage> swapchainImage);
        void Cleanup();

        virtual void Resize(uint32_t width, uint32_t height) override;

        virtual uint32_t GetColorAttachmentRenderID(uint32_t index = 0) const override;
        virtual const FramebufferProperties& GetProperties() const override { return properties; };

        inline const std::vector<Ref<VulkanImage>> GetImages() const { return attachmentImages; }
        inline const VkFramebuffer GetFramebuffer() const { return framebuffer; }
    private:
        VkFramebuffer framebuffer = VK_NULL_HANDLE;
        std::vector<Ref<VulkanImage>> attachmentImages;

        FramebufferProperties properties;
        std::vector<FramebufferTextureProperties> colorAttachmentProperties;
        std::vector<FramebufferTextureProperties> depthAttachmentProperties;
        std::vector<uint32_t> colorAttachments;
        std::vector<uint32_t> depthAttachment;
    };
}