#pragma once
#include <unordered_map>
#include "renderer/vulkan/VulkanShader.hpp"

namespace mist {
	class VulkanPipeline {
	public:
		VulkanPipeline() {}
		~VulkanPipeline() {}

		void Cleanup();
		void CreateGraphicsPipeline(const VulkanShader* shaderResources);

		VkPipeline& GetGraphicsPipeline(std::string name) { return pipelines[name]; }
		VkPipelineLayout& GetGraphicsPipelineLayout(std::string name) { return pipelineLayouts[name]; }
	private:
		std::unordered_map<std::string, VkPipelineLayout> pipelineLayouts;
		std::unordered_map<std::string, VkPipeline> pipelines;
	};
}