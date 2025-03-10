#include "renderer/Buffer.hpp"
#include "Debug.hpp"
#include "renderer/RenderCommand.hpp"
#include "renderer/vulkan/VulkanBuffer.hpp"

namespace mist {
    Ref<VertexBuffer> VertexBuffer::Create(uint32_t size) {
        switch (RenderCommand::GetAPIType()) {
        case RenderAPI::API::None:
            MIST_ASSERT(false, "None render API not supported");
            return nullptr;
        case RenderAPI::API::Vulkan:
            return CreateRef<VulkanVertexBuffer>(size);
        default:
            MIST_ASSERT(false, "Unknown render API");
            return nullptr;
        }
    }

    Ref<VertexBuffer> VertexBuffer::Create(float* vertices, uint32_t size) {
        switch (RenderCommand::GetAPIType())
        {
        case RenderAPI::API::None:
            MIST_ASSERT(false, "None render API not supported");
            return nullptr;
        case RenderAPI::API::Vulkan:
            return CreateRef<VulkanVertexBuffer>(vertices, size);
        default:
            MIST_ASSERT(false, "Unknown renderer API");
            return nullptr;
        }
    }

    Ref<IndexBuffer> IndexBuffer::Create(uint32_t* indices, uint32_t size) {
        switch (RenderCommand::GetAPIType())
        {
        case RenderAPI::API::None:
            MIST_ASSERT(false, "None render API not supported");
            return nullptr;
        case RenderAPI::API::Vulkan:
            return CreateRef<VulkanIndexBuffer>(indices, size);
        default:
            MIST_ASSERT(false, "Unknown renderer API");
            return nullptr;
        }
    }
}