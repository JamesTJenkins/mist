#include "VulkanDescriptors.hpp"
#include "renderer/vulkan/VulkanDebug.hpp"
#include "renderer/vulkan/VulkanContext.hpp"

namespace mist {
    void VulkanDescriptor::CreateEmptyDescriptorSetLayout() {
        VkDescriptorSetLayoutBinding uboLayout {};
        uboLayout.binding = 0;
        uboLayout.descriptorCount = 1;
        uboLayout.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        uboLayout.pImmutableSamplers = nullptr;
        uboLayout.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;

        VkDescriptorSetLayoutBinding samplerLayout {};
        samplerLayout.binding = 1;
        samplerLayout.descriptorCount = 1;
        samplerLayout.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        samplerLayout.pImmutableSamplers = nullptr;
        samplerLayout.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

        std::array<VkDescriptorSetLayoutBinding, 2> bindings = { uboLayout, samplerLayout };

        VkDescriptorSetLayoutCreateInfo layoutInfo {};
        layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
        layoutInfo.pBindings = bindings.data();

        VulkanContext& context = VulkanContext::GetContext();
        CheckVkResult(vkCreateDescriptorSetLayout(context.GetDevice(), &layoutInfo, nullptr, &descriptorSetLayout));
    }

    void VulkanDescriptor::CreateDescriptorSetLayout() {
        // TODO: come back later
    }

    void VulkanDescriptor::CreateEmptyDescriptorPool(uint32_t swapchainImagesCount) {
        VulkanContext& context = VulkanContext::GetContext();

        std::array<VkDescriptorPoolSize, 2> poolSizes {};
        poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        poolSizes[0].descriptorCount = swapchainImagesCount;
        poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        poolSizes[1].descriptorCount = swapchainImagesCount;

        VkDescriptorPoolCreateInfo poolInfo {};
        poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
        poolInfo.pPoolSizes = poolSizes.data();
        poolInfo.maxSets = swapchainImagesCount;

        CheckVkResult(vkCreateDescriptorPool(context.GetDevice(), &poolInfo, nullptr, &pool));
    }

    void VulkanDescriptor::CreateDescriptorPool() {
        // TODO: come back later
    }

    void VulkanDescriptor::CreateDescriptorSets(uint32_t swapchainImagesCount) {
        VulkanContext& context = VulkanContext::GetContext();

        std::vector<VkDescriptorSetLayout> layouts(swapchainImagesCount, descriptorSetLayout);

        VkDescriptorSetAllocateInfo allocInfo {};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = pool;
        allocInfo.descriptorSetCount = static_cast<uint32_t>(swapchainImagesCount);
        allocInfo.pSetLayouts = layouts.data();

        descriptorSets.resize(swapchainImagesCount);
        CheckVkResult(vkAllocateDescriptorSets(context.GetDevice(), &allocInfo, descriptorSets.data()));

        for (size_t i = 0; i < swapchainImagesCount; i++) {
            std::array<VkWriteDescriptorSet, 2> descriptorWrites {};

            VkDescriptorBufferInfo uboInfo {};
            // TODO: set uniform buffers up

            descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[0].dstSet = descriptorSets[i];
            descriptorWrites[0].dstBinding = 0;
            descriptorWrites[0].dstArrayElement = 0;
            descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            descriptorWrites[0].descriptorCount = 1;
            descriptorWrites[0].pBufferInfo = &uboInfo;

            std::vector<VkDescriptorImageInfo> imageInfos {};
            // TODO: set texture and sampler bindings in imageInfos

            descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[1].dstSet = descriptorSets[i];
            descriptorWrites[1].dstBinding = 1;
            descriptorWrites[1].dstArrayElement = 0;
            descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            descriptorWrites[1].descriptorCount = imageInfos.size();
            descriptorWrites[1].pBufferInfo = 0;
            descriptorWrites[1].pImageInfo = imageInfos.data();

            vkUpdateDescriptorSets(context.GetDevice(), static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
        }
    }

    void VulkanDescriptor::ClearPool() {
        if (pool != VK_NULL_HANDLE) {
            VulkanContext& context = VulkanContext::GetContext();
            vkDestroyDescriptorPool(context.GetDevice(), pool, nullptr);
        }
    }

    void VulkanDescriptor::Cleanup() {
        if (descriptorSetLayout != VK_NULL_HANDLE) {
            VulkanContext& context = VulkanContext::GetContext();
            vkDestroyDescriptorSetLayout(context.GetDevice(), descriptorSetLayout, nullptr);
        }
    }
}