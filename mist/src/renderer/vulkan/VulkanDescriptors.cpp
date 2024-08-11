#include "VulkanDescriptors.hpp"
#include "renderer/vulkan/VulkanDebug.hpp"
#include "renderer/vulkan/VulkanContext.hpp"

namespace mist {
	void VulkanDescriptor::CreateDescriptorPool() {
		// Initial pool sizes
		std::vector<VkDescriptorPoolSize> poolSize = {
			{ VK_DESCRIPTOR_TYPE_SAMPLER, 200 },
			{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 200 },
			{ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 200 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 100 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 200 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 200 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 200 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 200 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 200 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 200 },
			{ VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 200 }
		};

		VkDescriptorPoolCreateInfo info {};
		info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		info.poolSizeCount = static_cast<uint32_t>(poolSize.size());
		info.pPoolSizes = poolSize.data();
		info.maxSets = 1000 * (pools.size() + 1);	// Doubles the max pool size each time a new one is made
		info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;

		VulkanContext& context = VulkanContext::GetContext();
		VkDescriptorPool pool;
		CheckVkResult(vkCreateDescriptorPool(context.GetDevice(), &info, context.GetAllocationCallbacks(), &pool));

		pools.push_back(pool);
	}

	void VulkanDescriptor::CreateDescriptorSetLayout() {
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
		VkDescriptorSetLayout layout;
        CheckVkResult(vkCreateDescriptorSetLayout(context.GetDevice(), &layoutInfo, context.GetAllocationCallbacks(), &layout));

		descriptorSetLayouts.push_back(layout);
	}

	VkDescriptorSet& VulkanDescriptor::CreateDescriptorSet() {
		VkDescriptorSet descriptorSet;
		VulkanContext& context = VulkanContext::GetContext();

		// Try to place in existing pool
		for (VkDescriptorPool& pool : pools) {
			VkDescriptorSetAllocateInfo alloctionInfo {};
			alloctionInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
			alloctionInfo.descriptorPool = pool;
			alloctionInfo.descriptorSetCount = 1;
			//alloctionInfo.pSetLayouts = &descriptorSetLayout; // TODO: fix once shaders implemented

			VkResult result = vkAllocateDescriptorSets(context.GetDevice(), &alloctionInfo, &descriptorSet);
			switch(result) {
			case VK_SUCCESS:
				descriptorSets.push_back(descriptorSet);
				return descriptorSets[descriptorSets.size() - 1];
			case VK_ERROR_FRAGMENTED_POOL:
			case VK_ERROR_OUT_OF_POOL_MEMORY:
				continue;
			default:
				CheckVkResult(result);
			};
		}

		// Create new pool
		CreateDescriptorPool();
		
		VkDescriptorSetAllocateInfo alloctionInfo {};
		alloctionInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		alloctionInfo.descriptorPool = GetDescriptorPool(pools.size() - 1);
		alloctionInfo.descriptorSetCount = 1;
		//alloctionInfo.pSetLayouts = &descriptorSetLayout; // TODO: fix once shaders implemented

		CheckVkResult(vkAllocateDescriptorSets(context.GetDevice(), &alloctionInfo, &descriptorSet));

		descriptorSets.push_back(descriptorSet);
		return descriptorSets[descriptorSets.size() - 1];
	}

	void VulkanDescriptor::ClearPool() {
		VulkanContext& context = VulkanContext::GetContext();
		for (VkDescriptorPool& pool : pools) {
			vkDestroyDescriptorPool(context.GetDevice(), pool, context.GetAllocationCallbacks());
		}
	}

	void VulkanDescriptor::Cleanup() {
		VulkanContext& context = VulkanContext::GetContext();
		for (VkDescriptorSetLayout& layout : descriptorSetLayouts) {
			vkDestroyDescriptorSetLayout(context.GetDevice(), layout, nullptr);
		}
	}
}