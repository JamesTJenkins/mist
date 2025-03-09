#pragma once
#include <vector>
#include <vulkan/vulkan.h>

namespace mist {
    class VulkanCommand {
    public:
        VulkanCommand() {}
        ~VulkanCommand() {}

        void CreateCommandPool();
        void DestroyCommandPool();

        std::vector<VkCommandBuffer> AllocateCommandBuffers(uint32_t count);
        void FreeCommandBuffer(VkCommandBuffer commandBuffer);
        void FreeCommandBuffers(std::vector<VkCommandBuffer> commandBuffers);
    
        VkCommandBuffer BeginSingleTimeCommand();
        void BeginCommandBuffer(VkCommandBuffer commandBuffer);
        void EndCommandBuffer(VkCommandBuffer commandBuffer);

        void SubmitCommandBuffer(VkCommandBuffer* commandBuffer);
        void SubmitCommandBuffers(std::vector<VkCommandBuffer*> commandBuffers);
        void SubmitCommandBuffersImmediately(VkCommandBuffer* commandBuffers, uint32_t count);

        VkCommandBuffer* GetSubmittedBuffers() { return *submittedBuffers.data(); }
        size_t GetSubmittedBufferCount() { return submittedBuffers.size(); }
    private:
        VkCommandPool pool = VK_NULL_HANDLE;
        std::vector<VkCommandBuffer*> submittedBuffers;
    };
}