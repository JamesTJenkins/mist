#include "renderer/FrameBuffer.hpp"
#include "renderer/RenderCommand.hpp"
#include "renderer/vulkan/VulkanFrameBuffer.hpp"

namespace mist {
    Ref<FrameBuffer> FrameBuffer::Create(const FrameBufferProperties& properties) {
        switch (RenderCommand::GetAPIType()) {
        case RenderAPI::API::Vulkan:
            return CreateRef<VulkanFrameBuffer>(properties);
            break;
        default:
            MIST_ASSERT(false, "Unknown API");
            return nullptr;
        }
    }

    bool FrameBuffer::IsDepthFormat(FrameBufferTextureFormat format) {
        switch (format) {
            case FrameBufferTextureFormat::DEPTH24STENCIL8:
                return true;
            default:
                return false;
        }
    };
}