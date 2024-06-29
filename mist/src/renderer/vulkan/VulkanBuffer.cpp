#include "renderer/vulkan/VulkanBuffer.hpp"
#include "renderer/RenderCommand.hpp"
#include "renderer/vulkan/VulkanCommand.hpp"
#include "VulkanDebug.hpp"
#include "Log.hpp"
#include "renderer/vulkan/VulkanContext.hpp"

namespace mist {
    uint32_t FindMemoryType(uint32_t typeFiler, VkMemoryPropertyFlags flags) {
        VulkanContext& context = VulkanContext::GetContext();

        VkPhysicalDeviceMemoryProperties properties;
        vkGetPhysicalDeviceMemoryProperties(context.GetPhysicalDevice(), &properties);

        for (uint32_t i = 0; i < properties.memoryTypeCount; i++) {
            if (typeFiler & (1 << i) && (properties.memoryTypes[i].propertyFlags & flags) == flags)
                return i;
        }

        MIST_ERROR("Failed to find suitable memory type");
        return 0;
    }

    void CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags flags, VkBuffer& buffer, VkDeviceMemory& bufferMemory) {
        VulkanContext& context = VulkanContext::GetContext();

        VkBufferCreateInfo bufferInfo {};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = size;
        bufferInfo.usage = usage;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        CheckVkResult(vkCreateBuffer(context.GetDevice(), &bufferInfo, context.GetAllocationCallbacks(), &buffer));

        VkMemoryRequirements requirements;
        vkGetBufferMemoryRequirements(context.GetDevice(), buffer, &requirements);

        VkMemoryAllocateInfo memoryAllocInfo = {};
        memoryAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        memoryAllocInfo.allocationSize = requirements.size;
        memoryAllocInfo.memoryTypeIndex = FindMemoryType(requirements.memoryTypeBits, flags);

        CheckVkResult(vkAllocateMemory(context.GetDevice(), &memoryAllocInfo, context.GetAllocationCallbacks(), &bufferMemory));
        vkBindBufferMemory(context.GetDevice(), buffer, bufferMemory, 0);
    }

    void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size) {
        VulkanContext& context = VulkanContext::GetContext();

        VkCommandBuffer commandBuffer = context.commands.BeginSingleTimeCommand();
        
        VkBufferCopy copyRegion {};
        copyRegion.size = size;
        vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

        context.commands.EndCommandBuffer(commandBuffer);
        context.commands.SubmitCommandBuffers(&commandBuffer, 1);

        context.commands.FreeCommandBuffer(commandBuffer);
    }

    void SetBufferData(const void* data, uint32_t size, VkBuffer& buffer) {
        VulkanContext& context = VulkanContext::GetContext();

        VkBuffer stagingBuffer;
        VkDeviceMemory stagingMemory;
        CreateBuffer(size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingMemory);

        void* mappedData;
        vkMapMemory(context.GetDevice(), stagingMemory, 0, size, 0, &mappedData);
        std::memcpy(mappedData, data, size);
        vkUnmapMemory(context.GetDevice(), stagingMemory);

        CopyBuffer(stagingBuffer, buffer, size);

        vkDestroyBuffer(context.GetDevice(), stagingBuffer, context.GetAllocationCallbacks());
        vkFreeMemory(context.GetDevice(), stagingMemory, context.GetAllocationCallbacks());
    }

    VulkanVertexBuffer::VulkanVertexBuffer(uint32_t size) {
        CreateBuffer(size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, buffer, bufferMemory);
    }

    VulkanVertexBuffer::VulkanVertexBuffer(float* vertices, uint32_t size) {
        CreateBuffer(size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, buffer, bufferMemory);
        SetData(vertices, size);
    }

    VulkanVertexBuffer::~VulkanVertexBuffer() {
        VulkanContext& context = VulkanContext::GetContext();
        vkDestroyBuffer(context.GetDevice(), buffer, context.GetAllocationCallbacks());
        vkFreeMemory(context.GetDevice(), bufferMemory, context.GetAllocationCallbacks());
    }

    void VulkanVertexBuffer::Bind() const {
        vkCmdBindVertexBuffers(assignedBuffer, 0, 1, &buffer, 0);
    }

    void VulkanVertexBuffer::Unbind() const {
        VkBuffer nullBuffer = VK_NULL_HANDLE;
        VkDeviceSize nullOffset = 0;
        vkCmdBindVertexBuffers(assignedBuffer, 0, 1, &nullBuffer, &nullOffset);
    }

    void VulkanVertexBuffer::SetData(const void* data, uint32_t size) {
        SetBufferData(data, size, buffer);
    }

    VulkanIndexBuffer::VulkanIndexBuffer(uint32_t* indices, uint32_t count) : count(count) {
        const VkDeviceSize size = count * sizeof(uint32_t);
        CreateBuffer(size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, buffer, bufferMemory);
        SetBufferData(indices, size, buffer);
    }

    VulkanIndexBuffer::~VulkanIndexBuffer() {
        VulkanContext& context = VulkanContext::GetContext();
        vkDestroyBuffer(context.GetDevice(), buffer, context.GetAllocationCallbacks());
        vkFreeMemory(context.GetDevice(), bufferMemory, context.GetAllocationCallbacks());
    }

    void VulkanIndexBuffer::Bind() const {
        vkCmdBindVertexBuffers(assignedBuffer, 0, 1, &buffer, 0);
    }

    void VulkanIndexBuffer::Unbind() const {
        VkBuffer nullBuffer = VK_NULL_HANDLE;
        VkDeviceSize nullOffset = 0;
        vkCmdBindVertexBuffers(assignedBuffer, 0, 1, &nullBuffer, &nullOffset);
    }
}