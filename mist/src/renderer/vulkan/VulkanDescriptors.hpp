#pragma once
#include <vulkan/vulkan.h>
#include <vector>
#include "Debug.hpp"
#include "renderer/vulkan/VulkanShader.hpp"

// TODO: Create shader implementation and use SPIRV for reflection which will generate the descriptor set and layout
// gonna need to implement a way to track which shader to which descriptor layout and which decriptor layout for each sets

namespace mist {
	class VulkanDescriptor {
	public:
		VulkanDescriptor() {}
		~VulkanDescriptor() {}

		VkDescriptorSetLayout CreateDescriptorSetLayout(const VulkanShader& shader);
		void CreateDescriptorPool();
		VkDescriptorSet& CreateDescriptorSet();

		void ClearPool();
		void Cleanup();

		inline const VkDescriptorPool& GetDescriptorPool(const int index) const { return pools[index]; }
		inline const std::vector<VkDescriptorPool>& GetDescriptorPools() const { return pools; }
		inline const std::vector<VkDescriptorSetLayout>& GetDescriptorSetLayouts() const { return descriptorSetLayouts; }
		inline const std::vector<VkDescriptorSet>& GetDescriptorSets() const { return descriptorSets; }
	private:
		std::vector<VkDescriptorPool> pools;
		std::vector<VkDescriptorSetLayout> descriptorSetLayouts;
		std::vector<VkDescriptorSet> descriptorSets;
	};
}