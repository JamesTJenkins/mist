#pragma once
#include <vulkan/vulkan.h>
#include <vector>
#include <unordered_map>
#include <string>
#include "renderer/vulkan/VulkanShader.hpp"
#include "components/MeshRenderer.hpp"
#include "renderer/vulkan/VulkanBuffer.hpp"

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

	struct UniformBufferNameHash {
		size_t operator()(const std::pair<std::string, uint32_t>& p) const {
			size_t h1 = std::hash<std::string>{}(p.first);
			size_t h2 = std::hash<uint32_t>{}(p.second);
			return h1 ^ (h2 << 1);
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

		void UpdateDescriptorSetsWithUniformBuffers(const MeshRenderer& meshRenderer);
		void UpdateDescriptorSetsWithUniformBuffer(const std::string& name);
		
		template<typename T>
		void UpdateUniformBuffer(const std::string& name, T& data) {
			if (uniformBuffers.find(name) != uniformBuffers.end()) {
				UniformBuffer& buffer = uniformBuffers[name];
				buffer.SetData(sizeof(T), &data);
			}
		}
	private:
		bool Exists(const std::string& name) const { return descriptorSetLayouts.find(name) != descriptorSetLayouts.end(); }

		std::vector<VkDescriptorPool> pools;
		std::unordered_map<std::string, VkDescriptorSetLayout> descriptorSetLayouts;
		std::unordered_map<std::string, std::vector<VkDescriptorSetLayoutBinding>> descriptorSetLayoutBindings;
		std::unordered_map<DescriptorSetKey, VkDescriptorSet> descriptorSets;
		std::unordered_map<std::string, UniformBuffer> uniformBuffers;
		std::unordered_map<std::pair<std::string, uint32_t>, std::string, UniformBufferNameHash> uniformBufferNames;	// {shader name, binding} = uniformbuffer name
	};
}