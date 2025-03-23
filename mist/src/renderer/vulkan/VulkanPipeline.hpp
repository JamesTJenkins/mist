#pragma once
#include "renderer/vulkan/VulkanShader.hpp"

namespace mist {
	class VulkanPipeline {
	public:
		VulkanPipeline() {}
		~VulkanPipeline() {}

		void Cleanup();

		VkShaderModule CreateShaderModule(const std::vector<uint32_t> spirv);
		void CreateGraphicsPipeline(const VulkanShader* shaderResources);
	private:
		std::vector<VkPipelineLayout> pipelineLayouts;
		std::vector<VkPipeline> pipelines;
	};
}