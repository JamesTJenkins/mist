#pragma once
#include <vulkan/vulkan.h>
#include <vector>

namespace mist {
    class VulkanDescriptor {
    public:
        VulkanDescriptor() {}
        ~VulkanDescriptor() {}

        void CreateEmptyDescriptorSetLayout();
        void CreateDescriptorSetLayout();
        void CreateEmptyDescriptorPool();
        void CreateDescriptorPool();
        void CreateDescriptorSets();

        void ClearPool();
        void Cleanup();

        inline const VkDescriptorPool GetDescriptorPool() const { return pool; }
        inline const VkDescriptorSetLayout GetDescriptorSetLayout() const { return descriptorSetLayout; }
        inline const std::vector<VkDescriptorSet> GetDescriptorSets() const { return descriptorSets; }
    private:
        VkDescriptorPool pool;
        VkDescriptorSetLayout descriptorSetLayout;
        std::vector<VkDescriptorSet> descriptorSets;
    };
}