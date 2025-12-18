#include "VulkanContext.hpp"
#include <glm/glm.hpp>
#include <set>
#include <SDL3/SDL_vulkan.h>
#include "renderer/vulkan/VulkanDebug.hpp"
#include "Application.hpp"
#include "Debug.hpp"
#include "renderer/vulkan/VulkanHelper.hpp"

namespace mist {
	void FramebufferAttachment::Cleanup() {
		VulkanContext& context = VulkanContext::GetContext();

		if (view != VK_NULL_HANDLE)
			vkDestroyImageView(context.GetDevice(), view, context.GetAllocationCallbacks());

		if (image != VK_NULL_HANDLE)
			vmaDestroyImage(context.GetAllocator(), image, imageAlloc);
	}
	
	void FrameData::Cleanup() {
		VulkanContext& context = VulkanContext::GetContext();

		if (acquireImageSempahore != VK_NULL_HANDLE)
			vkDestroySemaphore(context.GetDevice(), acquireImageSempahore, context.GetAllocationCallbacks());
			
		if (inFlightFence != VK_NULL_HANDLE)
			vkDestroyFence(context.GetDevice(), inFlightFence, context.GetAllocationCallbacks());
	}

	const QueueFamilyIndices VulkanContext::FindQueueFamilies() const {
		QueueFamilyIndices indices;

		uint32_t count = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &count, NULL);
		VkQueueFamilyProperties* queues = (VkQueueFamilyProperties*)malloc(sizeof(VkQueueFamilyProperties) * count);
		vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &count, queues);

		for (uint32_t i = 0; i < count; i++) {
			if (queues[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
				indices.graphicsFamily = i;

			VkBool32 presentSupport = false;
			vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, surface, &presentSupport);

			if (presentSupport)
				indices.presentFamily = i;

			if (indices.Valid())
				break;
		}
		
		return indices;
	}

	uint32_t VulkanContext::FindMemoryType(uint32_t typeFiler, VkMemoryPropertyFlags flags) {
        VkPhysicalDeviceMemoryProperties properties;
        vkGetPhysicalDeviceMemoryProperties(physicalDevice, &properties);

        for (uint32_t i = 0; i < properties.memoryTypeCount; i++) {
            if (typeFiler & (1 << i) && (properties.memoryTypes[i].propertyFlags & flags) == flags)
                return i;
        }

        MIST_ERROR("Failed to find suitable memory type");
        return 0;
    }

	void VulkanContext::CreateInstance() {
		VkApplicationInfo appInfo = {};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName = Application::Get().GetApplicationName();
		appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.pEngineName = "mist";
		appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.apiVersion = VK_API_VERSION_1_3;

		std::vector<const char*> requestedExtensions = {};
		unsigned int count = 0;
		const char* const * sdlExtensions = SDL_Vulkan_GetInstanceExtensions(&count);
		requestedExtensions.reserve(count);
		for (size_t i = 0; i < count; i++) {
			requestedExtensions.emplace_back(sdlExtensions[i]);
		}

#ifdef DEBUG
		requestedExtensions.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif

		uint32_t extensionCount = 0;
		vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
		std::vector<VkExtensionProperties> availableExtensions(extensionCount);
		vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, availableExtensions.data());

		std::vector<const char*> validExtensions = {};
		for (const auto& extension : requestedExtensions) {
			bool found = false;
			for (const auto& availableExtension : availableExtensions) {
				if (strcmp(extension, availableExtension.extensionName) == 0) {
					validExtensions.push_back(extension);
					found = true;
					break;
				}
			}

			if (!found)
				MIST_WARN(std::string("Unavailable extension: ") + extension);
		}

		VkInstanceCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		createInfo.pApplicationInfo = &appInfo;
		createInfo.ppEnabledExtensionNames = validExtensions.data();
		createInfo.enabledExtensionCount = static_cast<uint32_t>(validExtensions.size());
		
#ifdef DEBUG
		uint32_t layerCount = 0;
		vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
		std::vector<VkLayerProperties> availableLayers(layerCount);
		vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

		std::vector<const char*> requestedLayers = {
			"VK_LAYER_KHRONOS_validation"
		};

		std::vector<const char*> validLayers = {};
		for (const auto& layer : requestedLayers) {
			bool found = false;
			for (const auto& availableLayer : availableLayers) {
				if (strcmp(layer, availableLayer.layerName) == 0) {
					validLayers.push_back(layer);
					found = true;
					break;
				}
			}

			if (!found)
				MIST_WARN(std::string("Unavailable layer: ") + layer);
		}

		createInfo.ppEnabledLayerNames = validLayers.data();
		createInfo.enabledLayerCount = static_cast<uint32_t>(validLayers.size());
#endif
		
		CheckVkResult(vkCreateInstance(&createInfo, allocationCallbacks, &instance));
		
#ifdef DEBUG
		PFN_vkCreateDebugUtilsMessengerEXT vkCreateDebugUtilsMessengerEXT = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");

		VkDebugUtilsMessengerCreateInfoEXT debugUtilsMessengerCI = {};
		debugUtilsMessengerCI.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		debugUtilsMessengerCI.messageSeverity =
		    VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT |
		    VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
		    VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT;
		debugUtilsMessengerCI.messageType =
		    VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
		    VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
		    VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		debugUtilsMessengerCI.pfnUserCallback = DebugUtilsMessengerCallback;
		debugUtilsMessengerCI.pUserData = nullptr;

		CheckVkResult(vkCreateDebugUtilsMessengerEXT(instance, &debugUtilsMessengerCI, allocationCallbacks, &debugMessenger));
#endif
	}

	void VulkanContext::CreateSurface() {
		if (!SDL_Vulkan_CreateSurface(Application::Get().GetWindow()->GetNativeWindow(), instance, allocationCallbacks, &surface))
			MIST_ERROR("Surface failed to be created");
	}

	void VulkanContext::CreatePhysicalDevice() {
		uint32_t gpuCount;
		CheckVkResult(vkEnumeratePhysicalDevices(instance, &gpuCount, NULL));
		MIST_ASSERT(gpuCount > 0, "Failed to find any GPUs");

		VkPhysicalDevice* gpus = (VkPhysicalDevice*)malloc(sizeof(VkPhysicalDevice) * gpuCount);
		CheckVkResult(vkEnumeratePhysicalDevices(instance, &gpuCount, gpus));

		// TODO: Add support for multi dedicated GPUs
		// Finds a discrete GPU
		uint32_t gpuIdx = 0;
		for (uint32_t i = 0; i < gpuCount; i++) {
			VkPhysicalDeviceProperties properties;
			vkGetPhysicalDeviceProperties(gpus[i], &properties);
			if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
				gpuIdx = i;
				break;
			}
		}

		physicalDevice = gpus[gpuIdx];
		free(gpus);
	}

	void VulkanContext::CreateDevice() {
		QueueFamilyIndices indicies = FindQueueFamilies();
		std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
		std::set<uint32_t> uniqueQueueFamilies = { indicies.graphicsFamily.value(), indicies.presentFamily.value() };
		float queuePriority = 1.0f;
		for (uint32_t queueFamily : uniqueQueueFamilies) {
			VkDeviceQueueCreateInfo createInfo{};
			createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			createInfo.queueFamilyIndex = queueFamily;
			createInfo.queueCount = 1;
			createInfo.pQueuePriorities = &queuePriority;
			queueCreateInfos.push_back(createInfo);
		}
		
		std::vector<const char*> deviceExtensions = {
			VK_KHR_SWAPCHAIN_EXTENSION_NAME
		};

		VkDeviceCreateInfo deviceCreateInfo{};
		deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
		deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();
		deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
		deviceCreateInfo.ppEnabledExtensionNames = deviceExtensions.data();
		CheckVkResult(vkCreateDevice(physicalDevice, &deviceCreateInfo, allocationCallbacks, &device));
		vkGetDeviceQueue(device, indicies.graphicsFamily.value(), 0, &graphicsQueue);
		vkGetDeviceQueue(device, indicies.presentFamily.value(), 0, &presentQueue);
	}

	void VulkanContext::CreateAllocator() {
		VmaAllocatorCreateInfo info {};
		info.instance = instance;
		info.physicalDevice = physicalDevice;
		info.device = device;

		CheckVkResult(vmaCreateAllocator(&info, &allocator));
	}

	void VulkanContext::CreateFrameDatas() {
		for (FrameData& data : frameDatas)
			data.Cleanup();
		frameDatas.clear();
		frameDatas.resize(MAX_FRAMES_IN_FLIGHT);

		VkSemaphoreCreateInfo semaphoreInfo {};
		semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		VkFenceCreateInfo fenceInfo{};
		fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
			CheckVkResult(vkCreateSemaphore(device, &semaphoreInfo, allocationCallbacks, &frameDatas[i].acquireImageSempahore));
			CheckVkResult(vkCreateFence(device, &fenceInfo, allocationCallbacks, &frameDatas[i].inFlightFence));
		}
	}

	void VulkanContext::Initialize() {
		CreateInstance();
		CreateSurface();
		CreatePhysicalDevice();
		CreateDevice();
		CreateAllocator();
		CreateCommandPool();
		AllocateCommandBuffers();
		CreateFrameDatas();
		MIST_INFO("Initialised Vulkan API");
	}

	void VulkanContext::Cleanup() {
		vkDeviceWaitIdle(device);

		for (VkFramebuffer& framebuffer : framebuffers)
			vkDestroyFramebuffer(device, framebuffer, allocationCallbacks);

		for (FramebufferAttachment& attachment : additionalFramebufferAttachments)
			attachment.Cleanup();

		if (renderPass != VK_NULL_HANDLE)
			vkDestroyRenderPass(device, renderPass, allocationCallbacks);

		for (VkImageView& swapchainImageView : swapchainImageViews)
			vkDestroyImageView(device, swapchainImageView, allocationCallbacks);

		for (VkSemaphore& semaphore : submitSemaphores)
			vkDestroySemaphore(device, semaphore, allocationCallbacks);

		if (swapchain != VK_NULL_HANDLE)
			vkDestroySwapchainKHR(device, swapchain, allocationCallbacks);

		if (commandPool != VK_NULL_HANDLE)
			vkDestroyCommandPool(device, commandPool, allocationCallbacks);

		for (FrameData& data : frameDatas)
			data.Cleanup();

		if (tempCommandBufferFence != VK_NULL_HANDLE)
			vkDestroyFence(device, tempCommandBufferFence, allocationCallbacks);

		vmaDestroyAllocator(allocator);
		vkDestroyDevice(device, allocationCallbacks);
		SDL_Vulkan_DestroySurface(instance, surface, allocationCallbacks);
#ifdef DEBUG
		PFN_vkDestroyDebugUtilsMessengerEXT vkDestroyDebugUtilsMessengerEXT = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
		vkDestroyDebugUtilsMessengerEXT(instance, debugMessenger, allocationCallbacks);
#endif
		vkDestroyInstance(instance, allocationCallbacks);
		MIST_INFO("Cleaned up Vulkan API");
	}

	std::vector<VkSurfaceFormatKHR> QuerySwapchainFormats(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface) {
		uint32_t count;
		vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &count, nullptr);
		std::vector<VkSurfaceFormatKHR> formats(count);
		vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &count, formats.data());
		return formats;
	}

	VkSurfaceFormatKHR ChooseSwapchainFormat(VkPhysicalDevice phyiscalDevice, VkSurfaceKHR surface, FramebufferProperties& properties) {
		bool validPreferredFormat = false;
		VkSurfaceFormatKHR preferedFormat;
		preferedFormat.colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;

		for (uint32_t i = 0; i < properties.attachment.attachmentsCount; i++) {
			if (VulkanHelper::IsColorFormat(properties.attachment.attachments[i].textureFormat)) {
				preferedFormat.format = VulkanHelper::GetVkFormat(properties.attachment.attachments[i].textureFormat);
				validPreferredFormat = true;
			}
		}
		
		std::vector<VkSurfaceFormatKHR> availableFormats = QuerySwapchainFormats(phyiscalDevice, surface);

		if (!validPreferredFormat) {
			MIST_INFO("Cant use preferred swapchain format using a fallback.");
			return availableFormats[0];
		}

		for (const VkSurfaceFormatKHR& format : availableFormats) {
			if (format.format == preferedFormat.format && format.colorSpace == preferedFormat.colorSpace) {
				return preferedFormat;
			}
		}

		MIST_INFO("Valid preferred swapchain format, but not supported on this system using a fallback.");
		return availableFormats[0];
	}

	VkPresentModeKHR ChooseSwapchainPresentMode(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, RenderAPI::VSYNC vsync) {
		VkPresentModeKHR preferredMode = VulkanHelper::GetPresentMode(vsync);

		uint32_t presentModeCount;
		vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, nullptr);
		std::vector<VkPresentModeKHR> availablePresentModes(presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, availablePresentModes.data());

		if (std::find(availablePresentModes.begin(), availablePresentModes.end(), preferredMode) != availablePresentModes.end())
			return preferredMode;

		return availablePresentModes[0];
	}

	VkExtent2D ChooseSwapchainExtent(VkSurfaceCapabilitiesKHR capabilities, FramebufferProperties& properties) {
		return {
			std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, properties.width)),
			std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, properties.height))
		};
	}

	VkFormat ChooseSwapchainDepthFormat(FramebufferProperties& properties) {
		bool validFormat = false;
		VkFormat preferredFormat;
		for (uint32_t i = 0; i < properties.attachment.attachmentsCount; i++) {
			if (VulkanHelper::IsDepthFormat(properties.attachment.attachments[i].textureFormat)) {
				preferredFormat = VulkanHelper::GetVkFormat(properties.attachment.attachments[i].textureFormat);
				validFormat = true;
			}
		}
		
		if (!validFormat) {
			MIST_INFO("No valid depth buffer selected, skipping depth buffer creation");
			return VK_FORMAT_UNDEFINED;
		}

		return VulkanHelper::FindSupportedDepthStencilFormat(preferredFormat, VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
	}

	VkAttachmentDescription CreateAttachmentDescription(size_t index, const FramebufferTextureProperties& textureProperties) {
		VkAttachmentDescription attachment {};
		attachment.format = VulkanHelper::GetVkFormat(textureProperties.textureFormat);
		attachment.samples = VK_SAMPLE_COUNT_1_BIT;
		attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		attachment.finalLayout = VulkanHelper::GetVkAttachmentDescriptionFinalLayout(index, textureProperties.textureFormat);

		return attachment;
	}

	void CreateRenderpass(VkDevice device, VkAllocationCallbacks* allocationCallbacks, FramebufferProperties& properties, uint32_t& colorAttachmentCount, VkRenderPass& renderPass) {
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

		VkSubpassDescription subpassInfo {};
		subpassInfo.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpassInfo.colorAttachmentCount = colorAttachmentCount;
		subpassInfo.pColorAttachments = colorAttachmentRefs.data();
		subpassInfo.pDepthStencilAttachment = depthAttachmentRefs.empty() ? nullptr : depthAttachmentRefs.data();

		VkRenderPassCreateInfo renderpassInfo {};
		renderpassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderpassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
		renderpassInfo.pAttachments = attachments.data();
		renderpassInfo.subpassCount = 1;
		renderpassInfo.pSubpasses = &subpassInfo;
		renderpassInfo.dependencyCount = 0;

		CheckVkResult(vkCreateRenderPass(device, &renderpassInfo, allocationCallbacks, &renderPass));
	}

	void VulkanContext::CreateSwapchain(FramebufferProperties& properties) {
		framebufferProperties = properties;

		VkSurfaceFormatKHR selectedFormat = ChooseSwapchainFormat(physicalDevice, surface, properties);
		
		VkSurfaceCapabilitiesKHR capabilities;
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &capabilities);

		uint32_t swapchainImageCount = capabilities.minImageCount;
		if (capabilities.maxImageCount > 0 && swapchainImageCount > capabilities.maxImageCount)
			swapchainImageCount = capabilities.maxImageCount;

		for (VkSemaphore& semaphore : submitSemaphores)
			vkDestroySemaphore(device, semaphore, allocationCallbacks);
		submitSemaphores.clear();
		submitSemaphores.resize(swapchainImageCount);

		VkSemaphoreCreateInfo semaphoreInfo {};
		semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
		for (size_t i = 0; i < swapchainImageCount; ++i)
			vkCreateSemaphore(device, &semaphoreInfo, allocationCallbacks, &submitSemaphores[i]);

		VkSwapchainKHR oldSwapchain = swapchain;

		extent = ChooseSwapchainExtent(capabilities, properties);

		VkSwapchainCreateInfoKHR swapchainInfo {};
		swapchainInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		swapchainInfo.surface = surface;
		swapchainInfo.imageFormat = selectedFormat.format;
		swapchainInfo.imageColorSpace = selectedFormat.colorSpace;
		swapchainInfo.presentMode = ChooseSwapchainPresentMode(physicalDevice, surface, Application::Get().GetRenderAPI()->GetVsyncMode());
		swapchainInfo.imageExtent = extent;
		swapchainInfo.minImageCount = swapchainImageCount;
		swapchainInfo.imageArrayLayers = 1;
		swapchainInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		swapchainInfo.preTransform = capabilities.currentTransform;
		swapchainInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		swapchainInfo.clipped = VK_TRUE;
		swapchainInfo.oldSwapchain = oldSwapchain;

		QueueFamilyIndices indicies = FindQueueFamilies();
		uint32_t queueFamilyIndices[] = { indicies.graphicsFamily.value(), indicies.presentFamily.value() };
		if (indicies.graphicsFamily != indicies.presentFamily) {
			swapchainInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			swapchainInfo.queueFamilyIndexCount = 2;
			swapchainInfo.pQueueFamilyIndices = queueFamilyIndices;
		} else {
			swapchainInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		}

		CheckVkResult(vkCreateSwapchainKHR(device, &swapchainInfo, allocationCallbacks, &swapchain));
		if (oldSwapchain != VK_NULL_HANDLE)
			vkDestroySwapchainKHR(device, oldSwapchain, allocationCallbacks);

		vkGetSwapchainImagesKHR(device, swapchain, &swapchainImageCount, nullptr);
		std::vector<VkImage> swapchainImages(swapchainImageCount);
		vkGetSwapchainImagesKHR(device, swapchain, &swapchainImageCount, swapchainImages.data());

		for (VkImageView view : swapchainImageViews)
			vkDestroyImageView(device, view, allocationCallbacks);
		swapchainImageViews.clear();
		swapchainImageViews.resize(swapchainImageCount);

		VkImageViewCreateInfo imageViewInfo {};
		imageViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		
		for (uint32_t i = 0; i < swapchainImageCount; ++i) {
			imageViewInfo.image = swapchainImages[i];
			imageViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			imageViewInfo.format = selectedFormat.format;
			imageViewInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
			imageViewInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
			imageViewInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
			imageViewInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
			imageViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			imageViewInfo.subresourceRange.baseMipLevel = 0;
			imageViewInfo.subresourceRange.levelCount = 1;
			imageViewInfo.subresourceRange.baseArrayLayer = 0;
			imageViewInfo.subresourceRange.layerCount = 1;
			CheckVkResult(vkCreateImageView(device, &imageViewInfo, allocationCallbacks, &swapchainImageViews[i]));
		}

		for (FramebufferAttachment& attachment : additionalFramebufferAttachments) {
			attachment.Cleanup();
		}
		additionalFramebufferAttachments.clear();

		int framebufferColorIndex;
		for (uint32_t i = 0; i < properties.attachment.attachmentsCount; ++i) {
			if (VulkanHelper::IsColorFormat(properties.attachment.attachments[i].textureFormat)) {
				framebufferColorIndex = i;
				break;
			}
		}
		additionalFramebufferAttachments.resize(properties.attachment.attachmentsCount - 1);

		for (size_t i = 0; i < properties.attachment.attachmentsCount; ++i) {
			// Skip as already used for swapchain
			if (framebufferColorIndex == i)
				continue;

			VkImageCreateInfo imageInfo {};
			imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
			imageInfo.imageType = VK_IMAGE_TYPE_2D;
			imageInfo.extent.width = extent.width;
			imageInfo.extent.height = extent.height;
			imageInfo.extent.depth = 1;
			imageInfo.mipLevels = 1;
			imageInfo.arrayLayers = 1;
			imageInfo.format = VulkanHelper::GetVkFormat(properties.attachment.attachments[i].textureFormat);
			imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
			imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			imageInfo.usage = VulkanHelper::IsDepthFormat(properties.attachment.attachments[i].textureFormat) ? VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT : VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
			imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
			imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
			imageInfo.flags = 0;

			VmaAllocationCreateInfo imageAllocInfo {};
			imageAllocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;

			additionalFramebufferAttachments[i] = {};
			CheckVkResult(vmaCreateImage(allocator, &imageInfo, &imageAllocInfo, &additionalFramebufferAttachments[i].image, &additionalFramebufferAttachments[i].imageAlloc, nullptr));
		
			VkImageViewCreateInfo imageViewInfo {};
			imageViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			imageViewInfo.image = additionalFramebufferAttachments[i].image;
			imageViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			imageViewInfo.format = VulkanHelper::GetVkFormat(properties.attachment.attachments[i].textureFormat);
			imageViewInfo.subresourceRange.aspectMask = VulkanHelper::IsDepthFormat(properties.attachment.attachments[i].textureFormat) ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT;
			imageViewInfo.subresourceRange.baseMipLevel = 0;
			imageViewInfo.subresourceRange.levelCount = 1;
			imageViewInfo.subresourceRange.baseArrayLayer = 0;
			imageViewInfo.subresourceRange.layerCount = 1;

			CheckVkResult(vkCreateImageView(device, &imageViewInfo, allocationCallbacks, &additionalFramebufferAttachments[i].view));
		}

		if (renderPass != VK_NULL_HANDLE)
			vkDestroyRenderPass(device, renderPass, allocationCallbacks);

		CreateRenderpass(device, allocationCallbacks, properties, colorAttachmentCount, renderPass);

		for (VkFramebuffer& framebuffer : framebuffers)
			vkDestroyFramebuffer(device, framebuffer, allocationCallbacks);
		framebuffers.clear();
		framebuffers.resize(swapchainImageViews.size());
		
		for (size_t i = 0; i < swapchainImages.size(); ++i) {
			std::vector<VkImageView> attachments = { swapchainImageViews[i] };
			attachments.resize(additionalFramebufferAttachments.size() + 1);

			for (FramebufferAttachment& additionalAttachments : additionalFramebufferAttachments) {
				attachments.push_back(additionalAttachments.view);
			}

			VkFramebufferCreateInfo framebufferInfo {};
			framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			framebufferInfo.renderPass = renderPass;
			framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
			framebufferInfo.pAttachments = attachments.data();
			framebufferInfo.width = extent.width;
			framebufferInfo.height = extent.height;
			framebufferInfo.layers = 1;
			
			CheckVkResult(vkCreateFramebuffer(device, &framebufferInfo, allocationCallbacks, &framebuffers[i]));
		}
	}

	void VulkanContext::RecreateSwapchain() {
		vkDeviceWaitIdle(device);
		CreateSwapchain(framebufferProperties);
	}

	void VulkanContext::CreateCommandPool() {
		if (commandPool != VK_NULL_HANDLE)
			vkDestroyCommandPool(device, commandPool, allocationCallbacks);
		
		VkCommandPoolCreateInfo poolInfo {};
		poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		poolInfo.queueFamilyIndex = FindQueueFamilies().graphicsFamily.value();
		poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		CheckVkResult(vkCreateCommandPool(device, &poolInfo, allocationCallbacks, &commandPool));
	}

	void VulkanContext::AllocateCommandBuffers() {
		commandBuffers.clear();
		commandBuffers.resize(MAX_FRAMES_IN_FLIGHT);
		
		VkCommandBufferAllocateInfo allocInfo {};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.commandPool = commandPool;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandBufferCount = MAX_FRAMES_IN_FLIGHT;

		CheckVkResult(vkAllocateCommandBuffers(device, &allocInfo, commandBuffers.data()));

		VkCommandBufferAllocateInfo tempAllocInfo {};
		tempAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		tempAllocInfo.commandPool = commandPool;
		tempAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		tempAllocInfo.commandBufferCount = 1;

		CheckVkResult(vkAllocateCommandBuffers(device, &tempAllocInfo, &tempCommandBuffer));

		if (tempCommandBufferFence != VK_NULL_HANDLE)
			vkDestroyFence(device, tempCommandBufferFence, allocationCallbacks);

		VkFenceCreateInfo tempCommandBufferFenceInfo{};
		tempCommandBufferFenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		CheckVkResult(vkCreateFence(device, &tempCommandBufferFenceInfo, allocationCallbacks, &tempCommandBufferFence));
	}

	void VulkanContext::BeginSingleTimeCommands() {
		CheckVkResult(vkResetCommandBuffer(tempCommandBuffer, 0));

		VkCommandBufferBeginInfo info {};
		info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		CheckVkResult(vkBeginCommandBuffer(tempCommandBuffer, &info));
	}

	void VulkanContext::EndSingleTimeCommands() {
		CheckVkResult(vkEndCommandBuffer(tempCommandBuffer));

		VkSubmitInfo info {};
		info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		info.commandBufferCount = 1;
		info.pCommandBuffers = &tempCommandBuffer;
		
		CheckVkResult(vkQueueSubmit(graphicsQueue, 1, &info, tempCommandBufferFence));
		CheckVkResult(vkWaitForFences(device, 1, &tempCommandBufferFence, VK_TRUE, UINT64_MAX));
		CheckVkResult(vkResetFences(device, 1, &tempCommandBufferFence));
	}

	void VulkanContext::BeginRenderPass() {
		CheckVkResult(vkWaitForFences(device, 1, &frameDatas[currentFrame].inFlightFence, VK_TRUE, UINT64_MAX));
 		CheckVkResult(vkResetFences(device, 1, &frameDatas[currentFrame].inFlightFence));

		CheckVkResult(vkAcquireNextImageKHR(device, swapchain, UINT64_MAX, frameDatas[currentFrame].acquireImageSempahore, VK_NULL_HANDLE, &imageIndex));

		CheckVkResult(vkResetCommandBuffer(commandBuffers[currentFrame], 0));
		VkCommandBufferBeginInfo cmdInfo {};
		cmdInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		cmdInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		CheckVkResult(vkBeginCommandBuffer(commandBuffers[currentFrame], &cmdInfo));

		VkRenderPassBeginInfo renderPassInfo {};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = renderPass;
		renderPassInfo.framebuffer = framebuffers[imageIndex];
		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = extent;
		renderPassInfo.clearValueCount = 1;
		glm::vec4 color = Application::Get().GetRenderAPI()->GetClearColor();
		VkClearValue clearColor = { color.r, color.g, color.b, color.a };
		renderPassInfo.pClearValues = &clearColor;

		vkCmdBeginRenderPass(commandBuffers[currentFrame], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
		
		// Using dynamic viewport and scissor so needs to be set every frame
		vkCmdSetViewport(commandBuffers[currentFrame], 0, 1, &viewport);
		vkCmdSetScissor(commandBuffers[currentFrame], 0, 1, &scissor);
	}

	void VulkanContext::EndRenderPass() {
		vkCmdEndRenderPass(commandBuffers[currentFrame]);
		CheckVkResult(vkEndCommandBuffer(commandBuffers[currentFrame]));

		VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = &frameDatas[currentFrame].acquireImageSempahore;
		submitInfo.pWaitDstStageMask = waitStages;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffers[currentFrame];
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = &submitSemaphores[imageIndex];

		CheckVkResult(vkQueueSubmit(graphicsQueue, 1, &submitInfo, frameDatas[currentFrame].inFlightFence));

		VkPresentInfoKHR presentInfo{};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = &submitSemaphores[imageIndex];
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = &swapchain;
		presentInfo.pImageIndices = &imageIndex;

		VkResult presentResult = vkQueuePresentKHR(presentQueue, &presentInfo);

		if (presentResult == VK_ERROR_OUT_OF_DATE_KHR) {
			RecreateSwapchain();
		} else if (presentResult != VK_SUCCESS && presentResult != VK_SUBOPTIMAL_KHR) {
			CheckVkResult(presentResult);
		}

		currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
	}
}