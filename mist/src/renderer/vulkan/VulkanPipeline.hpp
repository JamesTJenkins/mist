#pragma once
#include <vulkan/vulkan.h>
#include <unordered_map>
#include "renderer/vulkan/VulkanShader.hpp"
#include "renderer/vulkan/VulkanDescriptors.hpp"

namespace mist {
	class VulkanPipeline {
	public:
		VulkanPipeline() {}
		~VulkanPipeline() {}

		void Cleanup();
		void CreateGraphicsPipeline(const VulkanShader* shaderResources, const VkRenderPass& renderPass, const uint32_t colorAttachmentCount, VulkanDescriptor& descriptors);

		bool HasPipeline(const std::string name) { return pipelines.contains(name); }		
		VkPipeline& GetGraphicsPipeline(const std::string name) { return pipelines[name]; }
		VkPipelineLayout& GetGraphicsPipelineLayout(const std::string name) { return pipelineLayouts[name]; }
	private:
		std::unordered_map<std::string, VkPipelineLayout> pipelineLayouts;
		std::unordered_map<std::string, VkPipeline> pipelines;
	};
}