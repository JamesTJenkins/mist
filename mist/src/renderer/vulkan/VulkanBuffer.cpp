#include "renderer/vulkan/VulkanBuffer.hpp"

#include <vulkan/vulkan.h>
#include <renderer/vulkan/VulkanRenderAPI.hpp>
#include "renderer/RenderCommand.hpp"
#include "VulkanDebug.hpp"
#include "Log.hpp"

namespace mist {
    uint32_t FindMemoryType(VkPhysicalDevice physicalDevice, uint32_t typeFiler, VkMemoryPropertyFlags flags) {
        VkPhysicalDeviceMemoryProperties properties;
        vkGetPhysicalDeviceMemoryProperties(physicalDevice, &properties);

        for (uint32_t i = 0; i < properties.memoryTypeCount; i++) {
            if (typeFiler & (1 << i) && (properties.memoryTypes[i].propertyFlags & flags) == flags)
                return i;
        }

        MIST_ERROR("Failed to find suitable memory type");
        return 0;
    }

    void CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags flags, VkBuffer& buffer, VkDeviceMemory& bufferMemory) {
        const VulkanRenderAPI* api = dynamic_cast<VulkanRenderAPI*>(RenderCommand::GetAPI().get());

        VkBufferCreateInfo bufferInfo {};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = size;
        bufferInfo.usage = usage;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        CheckVkResult(vkCreateBuffer(api->GetDevice(), &bufferInfo, api->GetAllocationCallbacks(), &buffer));

        VkMemoryRequirements requirements;
        vkGetBufferMemoryRequirements(api->GetDevice(), buffer, &requirements);

        VkMemoryAllocateInfo memoryAllocInfo = {};
        memoryAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        memoryAllocInfo.allocationSize = requirements.size;
        memoryAllocInfo.memoryTypeIndex = FindMemoryType(api->GetPhysicalDevice(), requirements.memoryTypeBits, flags);

        CheckVkResult(vkAllocateMemory(api->GetDevice(), &memoryAllocInfo, api->GetAllocationCallbacks(), &bufferMemory));
        vkBindBufferMemory(api->GetDevice(), buffer, bufferMemory, 0);
    }

    void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size) {
        /*  TODO: Implement command buffers
        VkCommandBuffer commandBuffer = BeginSingleTimeCommand();
        
        VkBufferCopy copyRegion {};
        copyRegion.size = size;
        vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

        commandBuffer.EndSingleTimeCommand(commandBuffer);
        */

    }

    void SetBufferData(const void* data, uint32_t size, VkBuffer& buffer) {
        const VulkanRenderAPI* api = dynamic_cast<VulkanRenderAPI*>(RenderCommand::GetAPI().get());

        VkBuffer stagingBuffer;
        VkDeviceMemory stagingMemory;
        CreateBuffer(size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingMemory);

        void* mappedData;
        vkMapMemory(api->GetDevice(), stagingMemory, 0, size, 0, &mappedData);
        std::memcpy(mappedData, data, size);
        vkUnmapMemory(api->GetDevice(), stagingMemory);

        CopyBuffer(stagingBuffer, buffer, size);

        vkDestroyBuffer(api->GetDevice(), stagingBuffer, api->GetAllocationCallbacks());
        vkFreeMemory(api->GetDevice(), stagingMemory, api->GetAllocationCallbacks());
    }

    VulkanVertexBuffer::VulkanVertexBuffer(uint32_t size) {
        CreateBuffer(size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, buffer, bufferMemory);
    }

    VulkanVertexBuffer::VulkanVertexBuffer(float* vertices, uint32_t size) {
        CreateBuffer(size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, buffer, bufferMemory);
        SetData(vertices, size);
    }

    VulkanVertexBuffer::~VulkanVertexBuffer() {
        const VulkanRenderAPI* api = dynamic_cast<VulkanRenderAPI*>(RenderCommand::GetAPI().get());
        vkDestroyBuffer(api->GetDevice(), buffer, api->GetAllocationCallbacks());
        vkFreeMemory(api->GetDevice(), bufferMemory, api->GetAllocationCallbacks());
    }

    void VulkanVertexBuffer::Bind() const {

    }

    void VulkanVertexBuffer::Unbind() const {

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
        const VulkanRenderAPI* api = dynamic_cast<VulkanRenderAPI*>(RenderCommand::GetAPI().get());
        vkDestroyBuffer(api->GetDevice(), buffer, api->GetAllocationCallbacks());
        vkFreeMemory(api->GetDevice(), bufferMemory, api->GetAllocationCallbacks());
    }

    void VulkanIndexBuffer::Bind() const {

    }

    void VulkanIndexBuffer::Unbind() const {

    }
}