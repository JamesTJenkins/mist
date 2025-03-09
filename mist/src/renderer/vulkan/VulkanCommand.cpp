#include "VulkanCommand.hpp"
#include "renderer/RenderCommand.hpp"
#include "renderer/vulkan/VulkanDebug.hpp"
#include "renderer/vulkan/VulkanContext.hpp"

namespace mist {
    VkCommandBufferAllocateInfo CreateCommandBufferAllocationInfo(VkCommandPool pool, uint32_t count) {
        VkCommandBufferAllocateInfo info {};
        info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        info.commandPool = pool;
        info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        info.commandBufferCount = count;
        return info;
    }

    void VulkanCommand::CreateCommandPool() {
        VulkanContext& context = VulkanContext::GetContext();

        VkCommandPoolCreateInfo info {};
        info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        info.queueFamilyIndex = context.FindQueueFamilies().graphicsFamily.value();
        info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

        CheckVkResult(vkCreateCommandPool(context.GetDevice(), &info, context.GetAllocationCallbacks(), &pool));
    }

    void VulkanCommand::DestroyCommandPool() {
        VulkanContext& context = VulkanContext::GetContext();

        vkDestroyCommandPool(context.GetDevice(), pool, context.GetAllocationCallbacks());
    }

    std::vector<VkCommandBuffer> VulkanCommand::AllocateCommandBuffers(uint32_t count) {
        VulkanContext& context = VulkanContext::GetContext();

        std::vector<VkCommandBuffer> buffers(count);
        VkCommandBufferAllocateInfo info = CreateCommandBufferAllocationInfo(pool, count);
        CheckVkResult(vkAllocateCommandBuffers(context.GetDevice(), &info, buffers.data()));
        return buffers;
    }

    void VulkanCommand::FreeCommandBuffer(VkCommandBuffer commandBuffer) {
        VulkanContext& context = VulkanContext::GetContext();

        vkFreeCommandBuffers(context.GetDevice(), pool, 1, &commandBuffer);
    }

    void VulkanCommand::FreeCommandBuffers(std::vector<VkCommandBuffer> commandBuffers) {
        VulkanContext& context = VulkanContext::GetContext();

        vkFreeCommandBuffers(context.GetDevice(), pool, (uint32_t)commandBuffers.size(), commandBuffers.data());
    }

    VkCommandBuffer VulkanCommand::BeginSingleTimeCommand() {
        VulkanContext& context = VulkanContext::GetContext();

        VkCommandBuffer buffer;
        VkCommandBufferAllocateInfo info = CreateCommandBufferAllocationInfo(pool, 1);
        CheckVkResult(vkAllocateCommandBuffers(context.GetDevice(), &info, &buffer));
        return buffer;
    }

    void VulkanCommand::BeginCommandBuffer(VkCommandBuffer commandBuffer) {
        VkCommandBufferBeginInfo info {};
        info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        info.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;

        CheckVkResult(vkBeginCommandBuffer(commandBuffer, &info));
    }

    void VulkanCommand::EndCommandBuffer(VkCommandBuffer commandBuffer) {
        CheckVkResult(vkEndCommandBuffer(commandBuffer));
    }

    void VulkanCommand::SubmitCommandBuffer(VkCommandBuffer* commandBuffer) {
        submittedBuffers.push_back(commandBuffer);
    }

    void VulkanCommand::SubmitCommandBuffers(std::vector<VkCommandBuffer*> commandBuffers) {
        submittedBuffers.append_range(commandBuffers);
    }

    void VulkanCommand::SubmitCommandBuffersImmediately(VkCommandBuffer* commandBuffers, uint32_t count) {
        VkSubmitInfo info {};
        info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        info.commandBufferCount = count;
        info.pCommandBuffers = commandBuffers;

        VulkanContext& context = VulkanContext::GetContext();
        vkQueueSubmit(context.GetGraphicsQueue(), 1, &info, VK_NULL_HANDLE);
        vkQueueWaitIdle(context.GetGraphicsQueue());
    }
}