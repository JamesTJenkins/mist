#include "renderer/FrameBuffer.hpp"
#include "renderer/RenderCommand.hpp"
#include "renderer/vulkan/VulkanContext.hpp"
#include "Debug.hpp"

namespace mist {
    Ref<FrameBuffer> FrameBuffer::Create(const FrameBufferProperties& properties, const uint32_t swapchainInstance) {
        switch (RenderCommand::GetAPIType()) {
        case RenderAPI::API::Vulkan:
        {
            VulkanContext& context = VulkanContext::GetContext();
            if (context.GetSwapchainInstanceCount() > swapchainInstance) {
                context.GetSwapchainInstance(swapchainInstance).get()->CreateSwapchain(properties);
                return context.GetSwapchainInstance(swapchainInstance).get()->GetFrameBuffer();
            }

            MIST_ASSERT(context.GetSwapchainInstanceCount() - 1 == swapchainInstance || swapchainInstance == 0, "Instances are an index do 1 more than the current existing swapchains");
            context.CreateSwapchainInstance(properties);
            return context.GetSwapchainInstance(swapchainInstance).get()->GetFrameBuffer();
        }
        case RenderAPI::API::None:
            MIST_ASSERT(false, "Running headless");
            return nullptr;
        }

        return nullptr;
    }
}