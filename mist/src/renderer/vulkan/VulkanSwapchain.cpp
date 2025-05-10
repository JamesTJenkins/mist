#include "VulkanSwapchain.hpp"
#include "renderer/vulkan/VulkanDebug.hpp"
#include "renderer/vulkan/VulkanContext.hpp"
#include "renderer/vulkan/VulkanHelper.hpp"
#include "glm/glm.hpp"
#include "Application.hpp"
#include "renderer/Framebuffer.hpp"

namespace mist {
	// TODO: This function does not account for requested color space as currently just tries to get the requested format with VK_COLOR_SPACE_SRGB_NONLINEAR_KHR as the color space
	VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats, const VkFormat requestedFormat) {
		std::vector<VkSurfaceFormatKHR> preferredFormats = {
			{VK_FORMAT_B8G8R8A8_SRGB, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR},
			{VK_FORMAT_R8G8B8A8_SRGB, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR},
			{VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR},
			{VK_FORMAT_R8G8B8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR}
		};

		VkSurfaceFormatKHR chosen{};
		for (const VkSurfaceFormatKHR& preferredFormat : preferredFormats) {
			for (const VkSurfaceFormatKHR& availableFormat : availableFormats) {
				if (availableFormat.format == preferredFormat.format && availableFormat.colorSpace == preferredFormat.colorSpace) {
					chosen = availableFormat;
					if (chosen.format == requestedFormat) {
						return chosen;
					}
				}
			}
		}

		if (chosen.format != VK_FORMAT_UNDEFINED) {
			MIST_WARN(std::string("Cant use requested format, new format selected: ") + FramebufferTextureFormatToString(VulkanHelper::GetFramebufferTextureFormat(chosen.format)));
			return chosen;
		}

		MIST_WARN(std::string("Cant use any preferred formats, new format selected: ") + FramebufferTextureFormatToString(VulkanHelper::GetFramebufferTextureFormat(availableFormats[0].format)));
		return availableFormats[0];
	}

	VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR> availablePresentModes) {
		for (const VkPresentModeKHR& availableMode : availablePresentModes) {
			if (availableMode == VK_PRESENT_MODE_MAILBOX_KHR)
				return availableMode;
		}

		return availablePresentModes[0];
	}

	VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, unsigned int width, unsigned int height) {
		VkExtent2D size = { width, height };

		if (capabilities.currentExtent.width == 0xFFFFFFF) {
			size.width = glm::clamp<unsigned int>(size.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
			size.height = glm::clamp<unsigned int>(size.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);
		} else {
			size = capabilities.currentExtent;
		}

		return size;
	}

	VkAttachmentDescription CreateAttachmentDescription(size_t index, const FramebufferTextureProperties& textureProperties) {
		VkAttachmentDescription attachment {};
		attachment.format = VulkanHelper::GetVkFormat(textureProperties.textureFormat);
		attachment.samples = VK_SAMPLE_COUNT_1_BIT; // TODO: Add sample count to the texture properties then add here
		attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		attachment.finalLayout = VulkanHelper::GetVkAttachmentDescriptionFinalLayout(index, textureProperties.textureFormat);

		return attachment;
	}

	VulkanSwapchain::VulkanSwapchain() {}

	VulkanSwapchain::~VulkanSwapchain() {}
	
	const SwapchainSupportDetails VulkanSwapchain::QuerySwapchainSupport() const {
		VulkanContext& context = VulkanContext::GetContext();

		SwapchainSupportDetails details;
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(context.GetPhysicalDevice(), context.GetSurface(), &details.capabilities);

		uint32_t formatCount;
		vkGetPhysicalDeviceSurfaceFormatsKHR(context.GetPhysicalDevice(), context.GetSurface(), &formatCount, nullptr);
		if (formatCount != 0) {
			details.formats.resize(formatCount);
			vkGetPhysicalDeviceSurfaceFormatsKHR(context.GetPhysicalDevice(), context.GetSurface(), &formatCount, details.formats.data());
		}

		uint32_t presentModeCount;
		vkGetPhysicalDeviceSurfacePresentModesKHR(context.GetPhysicalDevice(), context.GetSurface(), &presentModeCount, nullptr);
		if (presentModeCount != 0) {
			details.presentMode.resize(presentModeCount);
			vkGetPhysicalDeviceSurfacePresentModesKHR(context.GetPhysicalDevice(), context.GetSurface(), &presentModeCount, details.presentMode.data());
		}

		return details;
	}

	VkRenderPass CreateNewRenderpass(const FramebufferProperties& properties, uint32_t& colorAttachmentCount) {
		std::vector<VkAttachmentDescription> attachments;
		std::vector<VkAttachmentReference> colorAttachmentRefs;
		std::vector<VkAttachmentReference> depthAttachmentRefs;

		for (size_t i = 0; i < properties.attachment.attachmentsCount; ++i) {
			VkAttachmentDescription attachment = CreateAttachmentDescription(i, properties.attachment.attachments[i]);
			attachments.push_back(attachment);

			bool depth = VulkanHelper::IsDepthFormat(properties.attachment.attachments[i].textureFormat);
			VkAttachmentReference ref {};
			ref.attachment = static_cast<uint32_t>(i);
			ref.layout = VulkanHelper::GetVkAttachmentDescriptionLayout(properties.attachment.attachments[i].textureFormat);

			if (depth) {
				depthAttachmentRefs.push_back(ref);
			} else {
				colorAttachmentRefs.push_back(ref);
			}
		}

		colorAttachmentCount = static_cast<uint32_t>(colorAttachmentRefs.size());

		// TODO: may need to add support for multiple subpasses at some point but 1 is good enough for now
		VkSubpassDescription subpassInfo {};
		subpassInfo.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpassInfo.colorAttachmentCount = colorAttachmentCount;
		subpassInfo.pColorAttachments = colorAttachmentRefs.data();
		subpassInfo.pDepthStencilAttachment = depthAttachmentRefs.empty() ? nullptr : depthAttachmentRefs.data();

		VkSubpassDependency dependency = {};
		dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		dependency.dstSubpass = 0;
		dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT; //| VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT; //| VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		dependency.srcAccessMask = 0;//VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
		dependency.dstStageMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT; //| VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
		dependency.dependencyFlags = 0;

		VkRenderPassCreateInfo renderpassInfo {};
		renderpassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderpassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
		renderpassInfo.pAttachments = attachments.data();
		renderpassInfo.subpassCount = 1;
		renderpassInfo.pSubpasses = &subpassInfo;
		renderpassInfo.dependencyCount = 1;
		renderpassInfo.pDependencies = &dependency;

		VulkanContext& context = VulkanContext::GetContext();
		VkRenderPass renderpass;
		CheckVkResult(vkCreateRenderPass(context.GetDevice(), &renderpassInfo, context.GetAllocationCallbacks(), &renderpass));
	
		return renderpass;
	}

	void VulkanSwapchain::ResizeSwapchain(uint32_t newWidth, uint32_t newHeight) {
		swapchainProperties.width = newWidth;
		swapchainProperties.height = newHeight;
		RecreateSwapchain();
	}

	void VulkanSwapchain::RecreateSwapchain() {
		CreateSwapchain(swapchainProperties);
	}

	void VulkanSwapchain::CreateSwapchain(FramebufferProperties& properties) {
		VulkanContext& context = VulkanContext::GetContext();
		vkDeviceWaitIdle(context.GetDevice());
		
		SwapchainSupportDetails swapchainSupport = QuerySwapchainSupport();

		VkSurfaceFormatKHR surfaceFormat = ChooseSwapSurfaceFormat(swapchainSupport.formats, VulkanHelper::GetVkFormat(properties.attachment.attachments[0].textureFormat));
		// Update properties [0] color format with swapchains newly selected one incase an issue arose
		if (VulkanHelper::GetFramebufferTextureFormat(surfaceFormat.format) != properties.attachment.attachments[0].textureFormat) {
			properties.attachment.attachments[0].textureFormat = VulkanHelper::GetFramebufferTextureFormat(surfaceFormat.format);
		}

		VkPresentModeKHR presentMode = ChooseSwapPresentMode(swapchainSupport.presentMode);
		VkExtent2D extent = ChooseSwapExtent(swapchainSupport.capabilities, Application::Get().GetWindow()->GetWidth(), Application::Get().GetWindow()->GetHeight());

		swapchainMinImageCount = swapchainSupport.capabilities.minImageCount;
		swapchainImageCount = swapchainMinImageCount + 1;

		if (swapchainSupport.capabilities.maxImageCount > 0 && swapchainImageCount > swapchainSupport.capabilities.maxImageCount)
			swapchainImageCount = swapchainSupport.capabilities.maxImageCount;

		VkImageUsageFlags usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;

		VkSwapchainKHR oldSwapchain = VK_NULL_HANDLE;
		if (swapchain != VK_NULL_HANDLE) {
			oldSwapchain = swapchain;
		}

		VkSwapchainCreateInfoKHR info {};
		info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		info.surface = context.GetSurface();
		info.minImageCount = swapchainImageCount;
		info.imageFormat = surfaceFormat.format;
		info.imageColorSpace = surfaceFormat.colorSpace;
		info.imageExtent = extent;
		info.imageArrayLayers = 1;
		info.imageUsage = usage;
		info.preTransform = swapchainSupport.capabilities.currentTransform;
		info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		info.presentMode = presentMode;
		info.clipped = VK_TRUE;
		info.oldSwapchain = oldSwapchain;

		QueueFamilyIndices indicies = context.FindQueueFamilies();
		uint32_t queueFamilyIndices[] = { indicies.graphicsFamily.value(), indicies.presentFamily.value() };

		if (indicies.graphicsFamily != indicies.presentFamily) {
			info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			info.queueFamilyIndexCount = 2;
			info.pQueueFamilyIndices = queueFamilyIndices;
		} else {
			info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
			info.queueFamilyIndexCount = 0;
			info.pQueueFamilyIndices = nullptr;
		}

		CheckVkResult(vkCreateSwapchainKHR(context.GetDevice(), &info, context.GetAllocationCallbacks(), &swapchain));

		if (oldSwapchain != VK_NULL_HANDLE) {
			vkDestroySwapchainKHR(context.GetDevice(), oldSwapchain, context.GetAllocationCallbacks());
		}

		swapchainImageFormat = surfaceFormat.format;
		swapchainExtent = extent;

		CleanupRenderPasses();
		CreateRenderPass(properties);

		vkGetSwapchainImagesKHR(context.GetDevice(), swapchain, &swapchainImageCount, nullptr);
		std::vector<VkImage> swapchainImages(swapchainImageCount);
		vkGetSwapchainImagesKHR(context.GetDevice(), swapchain, &swapchainImageCount, swapchainImages.data());

		VulkanImageProperties imageProps(
			swapchainImageFormat,
			VK_IMAGE_TILING_OPTIMAL,
			usage,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			VK_IMAGE_ASPECT_COLOR_BIT,
			extent.width,
			extent.height,
			1,
			1,
			true
		);

		framebuffers.clear();
		framebuffers.reserve(swapchainImageCount);
		for (size_t i = 0; i < swapchainImageCount; ++i) {
			framebuffers.push_back(CreateScope<VulkanFramebuffer>(
				properties, 
				renderpass, 
				CreateScope<VulkanImage>(swapchainImages[i], imageProps)
			));
		}

		swapchainProperties = properties;
	}

	void VulkanSwapchain::CreateRenderPass(const FramebufferProperties& properties) {
		renderpass = CreateNewRenderpass(properties, subpassColorAttachmentRefsCount);
	}

	void VulkanSwapchain::BeginRenderPass(VkCommandBuffer commandBuffer) {
		VkRenderPassBeginInfo renderPassInfo = {};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = renderpass;
		renderPassInfo.framebuffer = framebuffers[activeFramebuffer].get()->GetFramebuffer();
		renderPassInfo.renderArea.offset = { 0, 0 };
		FramebufferProperties props = framebuffers[activeFramebuffer].get()->GetProperties();
		renderPassInfo.renderArea.extent = { props.width, props.height };
		renderPassInfo.clearValueCount = 1;
		glm::vec4 color = Application::Get().GetRenderAPI()->GetClearColor();
		VkClearValue clearColor = { color.r, color.g, color.b, color.a };
		renderPassInfo.pClearValues = &clearColor;

		vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
	}

	void VulkanSwapchain::EndRenderPass(VkCommandBuffer commandBuffer) {
		vkCmdEndRenderPass(commandBuffer);
	}

	void VulkanSwapchain::CleanupFramebuffers() {
		framebuffers.clear();
	}

	void VulkanSwapchain::CleanupRenderPasses() {
		VulkanContext& context = VulkanContext::GetContext();
		vkDestroyRenderPass(context.GetDevice(), renderpass, context.GetAllocationCallbacks());
	}

	void VulkanSwapchain::CleanupSwapchain() {
		VulkanContext& context = VulkanContext::GetContext();
		vkDestroySwapchainKHR(context.GetDevice(), swapchain, context.GetAllocationCallbacks());
	}
}