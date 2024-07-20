#include "VulkanSwapchainInstance.hpp"
#include "renderer/vulkan/VulkanDebug.hpp"
#include "renderer/vulkan/VulkanContext.hpp"
#include "renderer/vulkan/VulkanHelper.hpp"
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

	VkAttachmentDescription CreateAttachmentDescription(FrameBufferTextureProperties textureProperties) {
		VkAttachmentDescription attachment {};
		attachment.format = VulkanHelper::IsDepthFormat(textureProperties.textureFormat) ? VulkanHelper::FindSupportedDepthFormat(VulkanHelper::GetVkFormat(textureProperties.textureFormat), VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) : VulkanHelper::GetVkFormat(textureProperties.textureFormat);
		attachment.samples = VK_SAMPLE_COUNT_1_BIT;
		attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		attachment.finalLayout = VulkanHelper::IsDepthFormat(textureProperties.textureFormat) ? VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL : VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		return attachment;
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

		VkImageUsageFlags usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;

		VulkanContext& context = VulkanContext::GetContext();

		VkSwapchainCreateInfoKHR info {};
		info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		info.surface = context.GetSurface();
		info.minImageCount = imageCount;
		info.imageFormat = surfaceFormat.format;
		info.imageColorSpace = surfaceFormat.colorSpace;
		info.imageExtent = extent;
		info.imageArrayLayers = 1;
		info.imageUsage = usage;
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

		// Update properties to account for the swapchain image format being possibly converted
		FrameBufferProperties newProps = properties;
		newProps.attachment.attachments[0].textureFormat = VulkanHelper::GetFrameBufferTextureFormat(swapchainImageFormat);
		CreateRenderPass(newProps);

		vkGetSwapchainImagesKHR(context.GetDevice(), swapchain, &imageCount, nullptr);
		std::vector<VkImage> swapchainImages(imageCount);
		vkGetSwapchainImagesKHR(context.GetDevice(), swapchain, &imageCount, swapchainImages.data());

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

		std::vector<VulkanImage> images(imageCount);
		for (size_t i = 0; i < imageCount; ++i) {
			VulkanImage image(swapchainImages[i], imageProps);
			image.CreateImageView();
			images[i] = image;
		}

		frameBuffers.clear();
		frameBuffers.resize(imageCount);
		for (size_t i = 0; i < imageCount; ++i) {
			frameBuffers[i] = CreateRef<VulkanFrameBuffer>(
				newProps, 
				renderpass, 
				images[i]
			);
		}
	}

	void VulkanSwapchainInstance::CreateRenderPass(const FrameBufferProperties& properties) {
		std::vector<VkAttachmentDescription> attachments;
		std::vector<VkAttachmentReference> colorAttachmentRefs;
		std::vector<VkAttachmentReference> depthAttachmentRefs;

		for (size_t i = 0; i < properties.attachment.attachments.size(); ++i) {
			VkAttachmentDescription attachment = CreateAttachmentDescription(properties.attachment.attachments[i]);
			attachments.push_back(attachment);

			bool depth = VulkanHelper::IsDepthFormat(properties.attachment.attachments[i].textureFormat);
			VkAttachmentReference ref {};
			ref.attachment = i;
			ref.layout = depth ? VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL : VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

			if (depth) {
				depthAttachmentRefs.push_back(ref);
			} else {
				colorAttachmentRefs.push_back(ref);
			}

			// TODO: may need to add support for multiple subpasses at some point but 1 is good enough for now
			VkSubpassDescription subpassInfo {};
			subpassInfo.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
			subpassInfo.colorAttachmentCount = static_cast<uint32_t>(colorAttachmentRefs.size());
			subpassInfo.pColorAttachments = colorAttachmentRefs.data();
			subpassInfo.pDepthStencilAttachment = depthAttachmentRefs.empty() ? nullptr : depthAttachmentRefs.data();

			VkRenderPassCreateInfo renderpassInfo {};
			renderpassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
			renderpassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
			renderpassInfo.pAttachments = attachments.data();
			renderpassInfo.subpassCount = 1;
			renderpassInfo.pSubpasses = &subpassInfo;

			VulkanContext& context = VulkanContext::GetContext();
			CheckVkResult(vkCreateRenderPass(context.GetDevice(), &renderpassInfo, context.GetAllocationCallbacks(), &renderpass));
		}
	}
}