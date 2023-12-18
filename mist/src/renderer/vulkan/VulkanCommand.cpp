#include "VulkanCommand.hpp"
#include "renderer/vulkan/VulkanRenderAPI.hpp"
#include "renderer/RenderCommand.hpp"
#include "renderer/vulkan/VulkanDebug.hpp"

namespace mist {
    VkCommandBufferAllocateInfo CreateCommandBufferAllocationInfo(VkCommandPool pool, uint32_t count) {
        VkCommandBufferAllocateInfo info {};
        info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        info.commandPool = pool;
        info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        info.commandBufferCount = count;
        return info;
    }

    VkCommandPool CreateCommandPool() {
        const VulkanRenderAPI* api = dynamic_cast<VulkanRenderAPI*>(RenderCommand::GetAPI().get());

        VkCommandPoolCreateInfo info {};
        info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        info.queueFamilyIndex = api->FindQueueFamilies().graphicsFamily.value();
        info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

        VkCommandPool pool;
        CheckVkResult(vkCreateCommandPool(api->GetDevice(), &info, api->GetAllocationCallbacks(), &pool));
        return pool;
    }

    void DestroyCommandPool(VkCommandPool pool) {
        const VulkanRenderAPI* api = dynamic_cast<VulkanRenderAPI*>(RenderCommand::GetAPI().get());
        vkDestroyCommandPool(api->GetDevice(), pool, api->GetAllocationCallbacks());
    }

    std::vector<VkCommandBuffer> AllocateCommandBuffers(VkCommandPool pool, uint32_t count) {
        const VulkanRenderAPI* api = dynamic_cast<VulkanRenderAPI*>(RenderCommand::GetAPI().get());
        std::vector<VkCommandBuffer> buffers(count);
        VkCommandBufferAllocateInfo info = CreateCommandBufferAllocationInfo(pool, count);
        CheckVkResult(vkAllocateCommandBuffers(api->GetDevice(), &info, buffers.data()));
        return buffers;
    }

    void FreeCommandBuffer(VkCommandPool pool, VkCommandBuffer commandBuffer) {
        const VulkanRenderAPI* api = dynamic_cast<VulkanRenderAPI*>(RenderCommand::GetAPI().get());
        vkFreeCommandBuffers(api->GetDevice(), pool, 1, &commandBuffer);
    }

    void FreeCommandBuffers(VkCommandPool pool, std::vector<VkCommandBuffer> commandBuffers) {
        const VulkanRenderAPI* api = dynamic_cast<VulkanRenderAPI*>(RenderCommand::GetAPI().get());
        vkFreeCommandBuffers(api->GetDevice(), pool, commandBuffers.size(), commandBuffers.data());
    }

    VkCommandBuffer BeginSingleTimeCommand(VkCommandPool pool) {
        const VulkanRenderAPI* api = dynamic_cast<VulkanRenderAPI*>(RenderCommand::GetAPI().get());
        VkCommandBuffer buffer;
        VkCommandBufferAllocateInfo info = CreateCommandBufferAllocationInfo(pool, 1);
        CheckVkResult(vkAllocateCommandBuffers(api->GetDevice(), &info, &buffer));
        return buffer;
    }

    void BeginCommandBuffer(VkCommandBuffer commandBuffer) {
        VkCommandBufferBeginInfo info {};
        info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        info.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;

        CheckVkResult(vkBeginCommandBuffer(commandBuffer, &info));
    }

    void EndCommandBuffer(VkCommandBuffer commandBuffer) {
        CheckVkResult(vkEndCommandBuffer(commandBuffer));
    }
}