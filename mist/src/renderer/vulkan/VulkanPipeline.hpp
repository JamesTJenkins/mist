#pragma once
#include "renderer/vulkan/VulkanShader.hpp"

namespace mist {
	class VulkanPipeline {
	public:
		VulkanPipeline() {}
		~VulkanPipeline() {}

		VkShaderModule CreateShaderModule(const std::vector<uint32_t> spirv);
		void CreateGraphicsPipeline(ShaderResource shaderResource);
	private:
	};
}