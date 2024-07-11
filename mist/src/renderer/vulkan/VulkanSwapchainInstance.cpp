#include "VulkanSwapchainInstance.hpp"
#include "renderer/vulkan/VulkanDebug.hpp"
#include "renderer/vulkan/VulkanContext.hpp"
#include "glm/glm.hpp"
#include "Application.hpp"

namespace mist {
	VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) {
		// TODO: look into this further, but this will do for now
		for (const VkSurfaceFormatKHR& availableFormats : availableFormats) {
			if (availableFormats.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormats.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
				return availableFormats;
		}

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

	VulkanSwapchainInstance::VulkanSwapchainInstance(const uint32_t swapchainIndex, const FrameBufferProperties& properties) : swapchainIndex(swapchainIndex) {
		CreateSwapchain(properties);
	}

	VulkanSwapchainInstance::~VulkanSwapchainInstance() {
		VulkanContext& context = VulkanContext::GetContext();
		vkDestroyRenderPass(context.GetDevice(), renderpass, context.GetAllocationCallbacks());
		vkDestroySwapchainKHR(context.GetDevice(), swapchain, context.GetAllocationCallbacks());
	}
	
	const SwapchainSupportDetails VulkanSwapchainInstance::QuerySwapchainSupport() const {
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

	void VulkanSwapchainInstance::CreateSwapchain(const FrameBufferProperties& properties) {
		SwapchainSupportDetails swapchainSupport = QuerySwapchainSupport();

		VkSurfaceFormatKHR surfaceFormat = ChooseSwapSurfaceFormat(swapchainSupport.formats);
		VkPresentModeKHR presentMode = ChooseSwapPresentMode(swapchainSupport.presentMode);
		VkExtent2D extent = ChooseSwapExtent(swapchainSupport.capabilities, Application::Get().GetWindow().GetWidth(), Application::Get().GetWindow().GetHeight());

		uint32_t imageCount = swapchainSupport.capabilities.minImageCount + 1;

		if (swapchainSupport.capabilities.maxImageCount > 0 && imageCount > swapchainSupport.capabilities.maxImageCount)
			imageCount = swapchainSupport.capabilities.maxImageCount;

		VulkanContext& context = VulkanContext::GetContext();

		VkSwapchainCreateInfoKHR info {};
		info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		info.surface = context.GetSurface();
		info.minImageCount = imageCount;
		info.imageFormat = surfaceFormat.format;
		info.imageColorSpace = surfaceFormat.colorSpace;
		info.imageExtent = extent;
		info.imageArrayLayers = 1;
		info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
		info.preTransform = swapchainSupport.capabilities.currentTransform;
		info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		info.presentMode = presentMode;
		info.clipped = VK_TRUE;
		info.oldSwapchain = VK_NULL_HANDLE;	// TODO: handle this properly

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

		swapchainImageFormat = surfaceFormat.format;
		swapchainExtent = extent;

		std::vector<VkImage> swapchainImages(imageCount);
		vkGetSwapchainImagesKHR(context.GetDevice(), swapchain, &imageCount, swapchainImages.data());

		std::vector<VulkanImageView> swapchainImageViews(imageCount);
		for (size_t i = 0; i < imageCount; ++i) {
			swapchainImageViews[i] = VulkanImageView(swapchainImages[i], swapchainImageFormat, VK_IMAGE_ASPECT_COLOR_BIT, 0);
		}

		frameBuffers.clear();
		frameBuffers.resize(imageCount);
		for (size_t i = 0; i < imageCount; ++i) {
			frameBuffers[i] = CreateRef<VulkanFrameBuffer>(
				properties, 
				swapchainIndex, 
				VulkanImage(swapchainImages[i]), 
				swapchainImageViews[i]
			);
		}
	}

	void VulkanSwapchainInstance::CreateRenderPass() {
		// Single color buffer
		VkAttachmentDescription colorAttachment{};
		colorAttachment.format = swapchainImageFormat;
		colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;	// TODO: This needs to be properly sort out at some point
		colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		colorAttachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		// Attachment ref (for layout(location = 0) out)
		VkAttachmentReference colorAttachmentRef{};
		colorAttachmentRef.attachment = 0;
		colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		// Depth
		VkAttachmentDescription depthAttachment{};
		depthAttachment.format = VulkanImageView::FindDepthFormat();
		depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;	// TODO: This needs to be properly sort out at some point
		depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		// Attachment ref (for layout(location = 1) out)
		VkAttachmentReference depthAttachmentRef{};
		depthAttachmentRef.attachment = 1;
		depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		// Resolve attachment
		VkAttachmentDescription colorAttachmentResolve{};
		colorAttachmentResolve.format = swapchainImageFormat;
		colorAttachmentResolve.samples = VK_SAMPLE_COUNT_1_BIT;
		colorAttachmentResolve.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachmentResolve.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		colorAttachmentResolve.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachmentResolve.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		colorAttachmentResolve.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		colorAttachmentResolve.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		// Attachment ref (for layout(location = 2) out)
		VkAttachmentReference colorAttachmentResolveRef{};
		colorAttachmentResolveRef.attachment = 2;
		colorAttachmentResolveRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		// TODO: whole subpass system may need a rework later on
		// Subpasses
		VkSubpassDescription subpass{};
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &colorAttachmentRef;
		subpass.pDepthStencilAttachment = &depthAttachmentRef;
		subpass.pResolveAttachments = &colorAttachmentResolveRef;

		// Subpass dependicies
		VkSubpassDependency dependency{};
		dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		dependency.dstSubpass = 0;
		dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		dependency.srcAccessMask = 0;
		dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

		std::vector<VkAttachmentDescription> attachments = {
			colorAttachment,
			depthAttachment,
			colorAttachmentResolve
		};

		VkRenderPassCreateInfo renderpassInfo{};
		renderpassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderpassInfo.attachmentCount = attachments.size();
		renderpassInfo.pAttachments = attachments.data();
		renderpassInfo.subpassCount = 1;
		renderpassInfo.pSubpasses = &subpass;
		renderpassInfo.dependencyCount = 1;
		renderpassInfo.pDependencies = &dependency;

		VulkanContext& context = VulkanContext::GetContext();
		CheckVkResult(vkCreateRenderPass(context.GetDevice(), &renderpassInfo, context.GetAllocationCallbacks(), &renderpass));
	}
}