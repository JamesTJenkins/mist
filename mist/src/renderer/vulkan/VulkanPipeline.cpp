#include "VulkanPipeline.hpp"
#include "renderer/vulkan/VulkanContext.hpp"
#include "VulkanDebug.hpp"

namespace mist {
	VkShaderModule VulkanPipeline::CreateShaderModule(const std::vector<uint32_t> spirv) {
		VkShaderModuleCreateInfo info{};
		info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		info.codeSize = spirv.size();
		info.pCode = spirv.data();

		VulkanContext& context = VulkanContext::GetContext();
		VkShaderModule module;
		CheckVkResult(vkCreateShaderModule(context.GetDevice(), &info, context.GetAllocationCallbacks(), &module));
		return module;
	}

	void VulkanPipeline::CreateGraphicsPipeline(ShaderResource shaderResource) {
		// After handling reflection of shader code pass through to here
		// going to have to generate all the configurations before they are used so at game launch or creating a cache file where all the shaders and variants are stored after compilation
		// Hold onto the pipeline in a unorderedmap/dictionary so the pipelines can be loaded when needed
		// read through this more https://zeux.io/2020/02/27/writing-an-efficient-vulkan-renderer/
	}
}

