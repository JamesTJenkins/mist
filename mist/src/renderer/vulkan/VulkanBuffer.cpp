#include "renderer/vulkan/VulkanBuffer.hpp"

#include <vulkan/vulkan.h>

namespace mist {
    VulkanVertexBuffer::VulkanVertexBuffer(uint32_t size) {

    }

    VulkanVertexBuffer::VulkanVertexBuffer(float* vertices, uint32_t size) {

    }

    VulkanVertexBuffer::~VulkanVertexBuffer() {

    }

    void VulkanVertexBuffer::Bind() const {

    }

    void VulkanVertexBuffer::Unbind() const {

    }

    void VulkanVertexBuffer::SetData(const void* data, uint32_t size) {

    }

    VulkanIndexBuffer::VulkanIndexBuffer(uint32_t* indices, uint32_t count) : count(count) {
        
    }

    VulkanIndexBuffer::~VulkanIndexBuffer() {

    }

    void VulkanIndexBuffer::Bind() const {

    }

    void VulkanIndexBuffer::Unbind() const {

    }
}