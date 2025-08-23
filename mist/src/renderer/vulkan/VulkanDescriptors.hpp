#pragma once
#include <vulkan/vulkan.h>
#include <vector>
#include <unordered_map>
#include <string>
#include "renderer/vulkan/VulkanShader.hpp"
#include "components/MeshRenderer.hpp"

namespace mist {
	struct DescriptorSetKey {
		VkDescriptorSetLayout layout;
		VkBuffer vertexBuffer;
		VkBuffer indexBuffer;

		bool operator==(const DescriptorSetKey& other) const {
			return layout == other.layout &&
				vertexBuffer == other.vertexBuffer &&
				indexBuffer == other.indexBuffer;
		}
	};
}

namespace std {
	template<>
	struct hash<mist::DescriptorSetKey> {
		size_t operator()(const mist::DescriptorSetKey& key) const {
			size_t h1 = hash<VkDescriptorSetLayout>{}(key.layout);
			size_t h2 = hash<VkBuffer>{}(key.vertexBuffer);
			size_t h3 = hash<VkBuffer>{}(key.indexBuffer);
			return h1 ^ (h2 << 1) ^ (h3 << 2);
		}
	};
}

namespace mist {
	class VulkanDescriptor {
	public:
		VulkanDescriptor() {}
		~VulkanDescriptor() {}

		VkDescriptorSetLayout CreateDescriptorSetLayout(const VulkanShader* shader);
		void CreateDescriptorPool();
		VkDescriptorSet& CreateDescriptorSet(const MeshRenderer& meshRenderer);

		void ClearPool();
		void Cleanup();

		inline const VkDescriptorPool& GetDescriptorPool(const int index) const { return pools[index]; }
		inline const std::vector<VkDescriptorPool>& GetDescriptorPools() const { return pools; }

		VkDescriptorSetLayout& GetDescriptorSetLayout(const std::string& name);
		VkDescriptorSet& GetDescriptorSet(const MeshRenderer& meshRenderer);
	private:
		bool Exists(const std::string& name) const { return descriptorSetLayouts.find(name) != descriptorSetLayouts.end(); }

		std::vector<VkDescriptorPool> pools;
		std::unordered_map<std::string, VkDescriptorSetLayout> descriptorSetLayouts;
		std::unordered_map<DescriptorSetKey, VkDescriptorSet> descriptorSets;
	};
}