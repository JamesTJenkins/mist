#include "VulkanPipeline.hpp"
#include "renderer/vulkan/VulkanContext.hpp"
#include "VulkanDebug.hpp"

namespace mist {
	void VulkanPipeline::Cleanup() {
		VulkanContext& context = VulkanContext::GetContext();

		for (size_t i = 0; i < pipelines.size(); i++) {
			vkDestroyPipeline(context.GetDevice(), pipelines[i], context.GetAllocationCallbacks());
		}
		
		for (size_t i = 0; i < pipelineLayouts.size(); i++) {
			vkDestroyPipelineLayout(context.GetDevice(), pipelineLayouts[i], context.GetAllocationCallbacks());
		}
	}

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

	void VulkanPipeline::CreateGraphicsPipeline(const VulkanShader* shader) {
		// After handling reflection of shader code pass through to here
		// going to have to generate all the configurations before they are used so at game launch or creating a cache file where all the shaders and variants are stored after compilation
		// Hold onto the pipeline in a unorderedmap/dictionary so the pipelines can be loaded when needed
		// read through this more https://zeux.io/2020/02/27/writing-an-efficient-vulkan-renderer/

		VulkanContext& context = VulkanContext::GetContext();
		FramebufferProperties framebufferProperties = context.GetSwapchain()->GetFrameBuffer()->GetProperties();

		VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
		vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertexInputInfo.vertexBindingDescriptionCount = 0;
		vertexInputInfo.pVertexBindingDescriptions = nullptr;
		vertexInputInfo.vertexAttributeDescriptionCount = 0;
		vertexInputInfo.pVertexAttributeDescriptions = nullptr;

		VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
		inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		inputAssembly.primitiveRestartEnable = VK_FALSE;

		VkViewport viewport;
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = (float)framebufferProperties.width / 2;
		viewport.height = (float)framebufferProperties.height / 2;
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		
		VkRect2D scissor{};
		scissor.offset = {0, 0};
		scissor.extent = VkExtent2D(framebufferProperties.width / 2, framebufferProperties.height / 2);

		VkPipelineViewportStateCreateInfo viewportState{};
		viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewportState.viewportCount = 1;
		viewportState.pViewports = &viewport;
		viewportState.scissorCount = 1;
		viewportState.pScissors = &scissor;

		VkPipelineRasterizationStateCreateInfo rasterizer{};
		rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		rasterizer.depthClampEnable = VK_FALSE;
		rasterizer.rasterizerDiscardEnable = VK_FALSE;
		rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
		rasterizer.lineWidth = 1.0f;
		rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
		rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
		rasterizer.depthBiasEnable = VK_FALSE;

		VkPipelineMultisampleStateCreateInfo multisampling{};
		multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multisampling.sampleShadingEnable = VK_FALSE;
		multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

		VkPipelineDepthStencilStateCreateInfo depthStencil{};
		depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		depthStencil.depthTestEnable = VK_TRUE;
		depthStencil.depthWriteEnable = VK_TRUE;
		depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
		depthStencil.depthBoundsTestEnable = VK_FALSE;
		depthStencil.stencilTestEnable = VK_FALSE;

		// 1 ColorBlendAttachmentState per attachment
		std::vector<VkPipelineColorBlendAttachmentState> colorBlendAttachmentStates(context.GetSwapchain()->GetSubpassColorAttachmentRefCount());
		for (VkPipelineColorBlendAttachmentState& state : colorBlendAttachmentStates) {
			state.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
			state.blendEnable = VK_FALSE;
			state.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
			state.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
			state.colorBlendOp = VK_BLEND_OP_ADD;
			state.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
			state.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
			state.alphaBlendOp = VK_BLEND_OP_ADD;
		}

		VkPipelineColorBlendStateCreateInfo colorBlending{};
		colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		colorBlending.logicOpEnable = VK_FALSE;
		colorBlending.logicOp = VK_LOGIC_OP_COPY;
		colorBlending.attachmentCount = context.GetSwapchain()->GetSubpassColorAttachmentRefCount();
		colorBlending.pAttachments = colorBlendAttachmentStates.data();
		colorBlending.blendConstants[0] = 0.0f;
		colorBlending.blendConstants[1] = 0.0f;
		colorBlending.blendConstants[2] = 0.0f;
		colorBlending.blendConstants[3] = 0.0f;

		VkDynamicState dynamicStates[] = {
			VK_DYNAMIC_STATE_VIEWPORT,
			VK_DYNAMIC_STATE_LINE_WIDTH
		};

		VkPipelineDynamicStateCreateInfo dynamicState{};
		dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		dynamicState.dynamicStateCount = 2;
		dynamicState.pDynamicStates = dynamicStates;

		VkDescriptorSetLayout layout = context.descriptors.CreateDescriptorSetLayout(shader);

		VkPipelineLayoutCreateInfo layoutInfo{};
		layoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		layoutInfo.setLayoutCount = 1;
		layoutInfo.pSetLayouts = &layout;

		VkPipelineLayout pipelineLayout;
		CheckVkResult(vkCreatePipelineLayout(context.GetDevice(), &layoutInfo, context.GetAllocationCallbacks(), &pipelineLayout));
		pipelineLayouts.push_back(pipelineLayout);

		std::vector<VkPipelineShaderStageCreateInfo> shaderStages;
		for (const auto& res : shader->GetResources()) {
			if (res.second.flags & VK_SHADER_STAGE_VERTEX_BIT) {
				VkPipelineShaderStageCreateInfo shaderStageInfo{};
				shaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
				shaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
				shaderStageInfo.module = res.second.shaderModule;
				shaderStageInfo.pName = res.first.c_str();
				shaderStages.push_back(shaderStageInfo);
			}

			if (res.second.flags & VK_SHADER_STAGE_FRAGMENT_BIT) {
				VkPipelineShaderStageCreateInfo shaderStageInfo{};
				shaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
				shaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
				shaderStageInfo.module = res.second.shaderModule;
				shaderStageInfo.pName = res.first.c_str();
				shaderStages.push_back(shaderStageInfo);
			}

			if (res.second.flags & VK_SHADER_STAGE_GEOMETRY_BIT) {
				VkPipelineShaderStageCreateInfo shaderStageInfo{};
				shaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
				shaderStageInfo.stage = VK_SHADER_STAGE_GEOMETRY_BIT;
				shaderStageInfo.module = res.second.shaderModule;
				shaderStageInfo.pName = res.first.c_str();
				shaderStages.push_back(shaderStageInfo);
			}

			if (res.second.flags & VK_SHADER_STAGE_COMPUTE_BIT) {
				VkPipelineShaderStageCreateInfo shaderStageInfo{};
				shaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
				shaderStageInfo.stage = VK_SHADER_STAGE_COMPUTE_BIT;
				shaderStageInfo.module = res.second.shaderModule;
				shaderStageInfo.pName = res.first.c_str();
				shaderStages.push_back(shaderStageInfo);
			}
		}

		VkGraphicsPipelineCreateInfo pipelineInfo{};
		pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipelineInfo.stageCount = static_cast<uint32_t>(shaderStages.size());
		pipelineInfo.pStages = shaderStages.data();
		pipelineInfo.pVertexInputState = &vertexInputInfo;
		pipelineInfo.pInputAssemblyState = &inputAssembly;
		pipelineInfo.pViewportState = &viewportState;
		pipelineInfo.pRasterizationState = &rasterizer;
		pipelineInfo.pMultisampleState = &multisampling;
		pipelineInfo.pDepthStencilState = &depthStencil;
		pipelineInfo.pColorBlendState = &colorBlending;
		pipelineInfo.pDynamicState = &dynamicState;
		pipelineInfo.layout = pipelineLayout;
		pipelineInfo.renderPass = context.GetSwapchain()->GetRenderPass();
		pipelineInfo.subpass = 0;
		pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

		VkPipeline graphicsPipeline;
		CheckVkResult(vkCreateGraphicsPipelines(context.GetDevice(), nullptr, 1, &pipelineInfo, context.GetAllocationCallbacks(), &graphicsPipeline));
	
		pipelines.push_back(graphicsPipeline);
	}
}

