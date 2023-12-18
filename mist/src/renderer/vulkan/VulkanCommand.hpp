#pragma once
#include <vector>
#include <vulkan/vulkan.h>

namespace mist {
    VkCommandPool CreateCommandPool();
    void DestroyCommandPool(VkCommandPool pool);

    std::vector<VkCommandBuffer> AllocateCommandBuffers(VkCommandPool pool, uint32_t count);
    void FreeCommandBuffer(VkCommandPool pool, VkCommandBuffer commandBuffer);
    void FreeCommandBuffers(VkCommandPool pool, std::vector<VkCommandBuffer> commandBuffers);
    
    VkCommandBuffer BeginSingleTimeCommand(VkCommandPool pool);
    void BeginCommandBuffer(VkCommandBuffer commandBuffer);
    void EndCommandBuffer(VkCommandBuffer commandBuffer);

    void SubmitCommandBuffers(VkCommandBuffer* commandBuffers, uint32_t count);
}