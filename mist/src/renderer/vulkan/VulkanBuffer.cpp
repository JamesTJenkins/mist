#include "renderer/vulkan/VulkanBuffer.hpp"

#include <vulkan/vulkan.h>
#include <renderer/vulkan/VulkanRenderAPI.hpp>

namespace mist {
    VulkanVertexBuffer::VulkanVertexBuffer(uint32_t size) {

    }

    VulkanVertexBuffer::VulkanVertexBuffer(float* vertices, uint32_t size) {
        VkBufferCreateInfo bufferInfo {};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = size;
        //bufferInfo.usage = VkBufferUsageFlags
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        // TODO: some how do vkCreateBuffer without it being messy
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