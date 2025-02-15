#include "renderer/Framebuffer.hpp"
#include "renderer/RenderCommand.hpp"
#include "renderer/vulkan/VulkanContext.hpp"
#include "Debug.hpp"

namespace mist {
    Ref<Framebuffer> Framebuffer::Create(const FramebufferProperties& properties) {
        switch (RenderCommand::GetAPIType()) {
        case RenderAPI::API::Vulkan:
        {
            VulkanContext& context = VulkanContext::GetContext();
            if (context.GetSwapchain() != nullptr) {
                context.GetSwapchain()->CreateSwapchain(properties);
                return context.GetSwapchain()->GetFrameBuffer();
            }
            
            MIST_INFO("Creating new swapchain for framebuffer");
            context.CreateSwapchain(properties);
            return context.GetSwapchain()->GetFrameBuffer();
        }
        case RenderAPI::API::None:
            MIST_ASSERT(false, "Running headless");
            return nullptr;
        }

        return nullptr;
    }
}