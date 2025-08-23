#include "VulkanDescriptors.hpp"
#include "renderer/vulkan/VulkanDebug.hpp"
#include "renderer/vulkan/VulkanContext.hpp"
#include "renderer/vulkan/VulkanBuffer.hpp"
#include "Debug.hpp"

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
		info.maxSets = (uint32_t)(1000 * (pools.size() + 1));	// Doubles the max pool size each time a new one is made
		info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;

		VulkanContext& context = VulkanContext::GetContext();
		VkDescriptorPool pool;
		CheckVkResult(vkCreateDescriptorPool(context.GetDevice(), &info, context.GetAllocationCallbacks(), &pool));

		pools.push_back(pool);
	}

	VkDescriptorSetLayout VulkanDescriptor::CreateDescriptorSetLayout(const VulkanShader* shader) {
		std::vector<VkDescriptorSetLayoutBinding> layoutBindings;

		for (const auto& res : shader->GetUboResources()) {
			VkDescriptorSetLayoutBinding layoutBinding{};
			layoutBinding.binding = res.second.binding;
			layoutBinding.descriptorType = res.second.type;
			layoutBinding.descriptorCount = res.second.count;
			layoutBinding.stageFlags = res.second.flags;
			layoutBinding.pImmutableSamplers = nullptr;
			layoutBindings.push_back(layoutBinding);
		}

        VkDescriptorSetLayoutCreateInfo layoutInfo{};
		layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutInfo.bindingCount = static_cast<uint32_t>(layoutBindings.size());
		layoutInfo.pBindings = layoutBindings.data();
		
		VulkanContext& context = VulkanContext::GetContext();
		VkDescriptorSetLayout layout;
        CheckVkResult(vkCreateDescriptorSetLayout(context.GetDevice(), &layoutInfo, context.GetAllocationCallbacks(), &layout));

		descriptorSetLayouts[shader->GetName()] = layout;
		return layout;
	}

	VkDescriptorSet& VulkanDescriptor::CreateDescriptorSet(const MeshRenderer& meshRenderer) {
		VkDescriptorSet descriptorSet;
		VulkanContext& context = VulkanContext::GetContext();

		DescriptorSetKey key{};
		key.layout = GetDescriptorSetLayout(meshRenderer.shaderName);
		key.vertexBuffer = std::static_pointer_cast<VulkanVertexBuffer>(meshRenderer.vBuffer)->GetBuffer();
		key.indexBuffer = std::static_pointer_cast<VulkanIndexBuffer>(meshRenderer.iBuffer)->GetBuffer();

		// Try to place in existing pool
		for (VkDescriptorPool& pool : pools) {
			VkDescriptorSetAllocateInfo allocationInfo {};
			allocationInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
			allocationInfo.descriptorPool = pool;
			allocationInfo.descriptorSetCount = 1;
			allocationInfo.pSetLayouts = &GetDescriptorSetLayout(meshRenderer.shaderName);

			VkResult result = vkAllocateDescriptorSets(context.GetDevice(), &allocationInfo, &descriptorSet);

			switch(result) {
			case VK_SUCCESS:
				descriptorSets[key] = descriptorSet;
				return descriptorSets[key];
			case VK_ERROR_FRAGMENTED_POOL:
			case VK_ERROR_OUT_OF_POOL_MEMORY:
				continue;
			default:
				CheckVkResult(result);
			};
		}

		CreateDescriptorPool();
		
		VkDescriptorSetAllocateInfo alloctionInfo {};
		alloctionInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		alloctionInfo.descriptorPool = GetDescriptorPool((int)pools.size() - 1);
		alloctionInfo.descriptorSetCount = 1;
		alloctionInfo.pSetLayouts = &GetDescriptorSetLayout(meshRenderer.shaderName);

		CheckVkResult(vkAllocateDescriptorSets(context.GetDevice(), &alloctionInfo, &descriptorSet));

		descriptorSets[key] = descriptorSet;
		return descriptorSets[key];
	}

	void VulkanDescriptor::ClearPool() {
		VulkanContext& context = VulkanContext::GetContext();
		for (VkDescriptorPool& pool : pools) {
			vkDestroyDescriptorPool(context.GetDevice(), pool, context.GetAllocationCallbacks());
		}
	}

	void VulkanDescriptor::Cleanup() {
		VulkanContext& context = VulkanContext::GetContext();
		for (std::pair<std::string, VkDescriptorSetLayout> layout : descriptorSetLayouts) {
			vkDestroyDescriptorSetLayout(context.GetDevice(), layout.second, nullptr);
		}
	}

	VkDescriptorSetLayout& VulkanDescriptor::GetDescriptorSetLayout(const std::string& name) {
		MIST_ASSERT(Exists(name), "Descriptor set layout not found"); 
		return descriptorSetLayouts[name];
	}

	VkDescriptorSet& VulkanDescriptor::GetDescriptorSet(const MeshRenderer& meshRenderer) {
		DescriptorSetKey key{};
		key.layout = GetDescriptorSetLayout(meshRenderer.shaderName);
		key.vertexBuffer = std::static_pointer_cast<VulkanVertexBuffer>(meshRenderer.vBuffer)->GetBuffer();
		key.indexBuffer = std::static_pointer_cast<VulkanIndexBuffer>(meshRenderer.iBuffer)->GetBuffer();

		auto it = descriptorSets.find(key);
		if (it != descriptorSets.end())
			return it->second;

		return CreateDescriptorSet(meshRenderer);
	}
}