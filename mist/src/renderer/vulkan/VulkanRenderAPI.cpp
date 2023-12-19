#include "renderer/vulkan/VulkanRenderAPI.hpp"
#include <vector>
#include <set>
#include <SDL2/SDL_vulkan.h>
#include "renderer/vulkan/VulkanDebug.hpp"
#include "Application.hpp"
#include "VulkanRenderAPI.hpp"

namespace mist {
	const QueueFamilyIndices VulkanRenderAPI::FindQueueFamilies() const {
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

	const SwapchainSupportDetails VulkanRenderAPI::QuerySwapchainSupport() const {
		SwapchainSupportDetails details;
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &details.capabilities);

		uint32_t formatCount;
		vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, nullptr);
		if (formatCount != 0) {
			details.formats.resize(formatCount);
			vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, details.formats.data());
		}

		uint32_t presentModeCount;
		vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, nullptr);
		if (presentModeCount != 0) {
			details.presentMode.resize(presentModeCount);
			vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, details.presentMode.data());
		}

		return details;
	}

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

	void VulkanRenderAPI::Initialize() {
		VkResult error;

		// VULKAN INSTANCE
		CreateInstance();
		// SURFACE
		CreateSurface();
		// SELECT GPU
		{
			uint32_t gpuCount;
			error = vkEnumeratePhysicalDevices(instance, &gpuCount, NULL);
			VkPhysicalDevice* gpus = (VkPhysicalDevice*)malloc(sizeof(VkPhysicalDevice) * gpuCount);
			error = vkEnumeratePhysicalDevices(instance, &gpuCount, gpus);
			CheckVkResult(error);

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
		// Queues and device
		{
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

			error = vkCreateDevice(physicalDevice, &deviceCreateInfo, allocator, &device);
			CheckVkResult(error);
			vkGetDeviceQueue(device, indicies.graphicsFamily.value(), 0, &graphicsQueue);
			vkGetDeviceQueue(device, indicies.presentFamily.value(), 0, &presentQueue);
		}
		// Create descriptor
		{
			// TODO: REWORK THIS
			VkDescriptorPoolSize poolSize[] = {
				{ VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
				{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
				{ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
				{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
				{ VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
				{ VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
				{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
				{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
				{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
				{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
				{ VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
			};
			VkDescriptorPoolCreateInfo poolCreateInfo{};
			poolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
			poolCreateInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
			poolCreateInfo.maxSets = 1000 * sizeof(VkDescriptorPoolSize);
			poolCreateInfo.pPoolSizes = poolSize;
			error = vkCreateDescriptorPool(device, &poolCreateInfo, allocator, &descriptorPool);
			CheckVkResult(error);
		}
		// SWAPCHAIN
		CreateSwapchain();
	}

	void VulkanRenderAPI::Shutdown() {
		vkDestroySwapchainKHR(device, swapchain, allocator);
		vkDestroyDescriptorPool(device, descriptorPool, allocator);
		vkDestroyDevice(device, allocator);
		vkDestroySurfaceKHR(instance, surface, allocator);
#ifdef DEBUG
		auto vkDestroyDebugReportCallbackEXT = (PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugReportCallbackEXT");
		vkDestroyDebugReportCallbackEXT(instance, debugReport, allocator);
#endif
		vkDestroyInstance(instance, allocator);
	}

	void VulkanRenderAPI::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) {
		
	}

	void VulkanRenderAPI::SetClearColor(glm::vec4& color) {

	}

	void VulkanRenderAPI::Clear() {

	}

	void VulkanRenderAPI::CreateInstance() {
		VkApplicationInfo appInfo = {};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName = "Untitled";
		appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.pEngineName = "mist";
		appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.apiVersion = VK_API_VERSION_1_0;

#ifdef DEBUG
		std::vector<const char*> extensions = {
			VK_EXT_DEBUG_REPORT_EXTENSION_NAME
		};
#else
		std::vector<const char*> extensions = {};
#endif

		unsigned int count = 0;
		SDL_Vulkan_GetInstanceExtensions((SDL_Window*)Application::Get().GetWindow().GetNativeWindow(), &count, nullptr);
		size_t additionalExtensionCount = extensions.size();
		extensions.resize(additionalExtensionCount + count);
		SDL_Vulkan_GetInstanceExtensions((SDL_Window*)Application::Get().GetWindow().GetNativeWindow(), &count, extensions.data() + additionalExtensionCount);
			
		VkInstanceCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		createInfo.pApplicationInfo = &appInfo;
		createInfo.ppEnabledExtensionNames = extensions.data();
		createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
			
#ifdef DEBUG
		const char* layers[] = {
			"VK_LAYER_KHRONOS_validation"
		};

		createInfo.ppEnabledLayerNames = layers;
		createInfo.enabledLayerCount = 1;
#endif

		CheckVkResult(vkCreateInstance(&createInfo, allocator, &instance));

#ifdef DEBUG
		auto vkCreateDebugReportCallbackEXT = (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugReportCallbackEXT");
		VkDebugReportCallbackCreateInfoEXT debug_report_ci = {};
		debug_report_ci.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
		debug_report_ci.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT | VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT;
		debug_report_ci.pfnCallback = DebugReport;
		debug_report_ci.pUserData = NULL;
		CheckVkResult(vkCreateDebugReportCallbackEXT(instance, &debug_report_ci, allocator, &debugReport));
#endif
	}

	void VulkanRenderAPI::CreateSurface() {
		if (!SDL_Vulkan_CreateSurface((SDL_Window*)Application::Get().GetWindow().GetNativeWindow(), instance, &surface))
			printf("[VULKAN] surface failed to be created");
	}

	void VulkanRenderAPI::CreateSwapchain() {
        SwapchainSupportDetails swapchainSupport = QuerySwapchainSupport();

        VkSurfaceFormatKHR surfaceFormat = ChooseSwapSurfaceFormat(swapchainSupport.formats);
        VkPresentModeKHR presentMode = ChooseSwapPresentMode(swapchainSupport.presentMode);
        VkExtent2D extent = ChooseSwapExtent(swapchainSupport.capabilities, Application::Get().GetWindow().GetWidth(), Application::Get().GetWindow().GetHeight());

        uint32_t imageCount = swapchainSupport.capabilities.minImageCount + 1;

        if (swapchainSupport.capabilities.maxImageCount > 0 && imageCount > swapchainSupport.capabilities.maxImageCount)
            imageCount = swapchainSupport.capabilities.maxImageCount;

        VkSwapchainCreateInfoKHR info {};
        info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        info.surface = surface;
        info.minImageCount = imageCount;
        info.imageFormat = surfaceFormat.format;
        info.imageColorSpace = surfaceFormat.colorSpace;
        info.imageExtent = extent;
        info.imageArrayLayers = 1;
        info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        info.preTransform = swapchainSupport.capabilities.currentTransform;
        info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        info.presentMode = presentMode;
        info.clipped = VK_TRUE;
        info.oldSwapchain = VK_NULL_HANDLE;

        QueueFamilyIndices indicies = FindQueueFamilies();
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

        CheckVkResult(vkCreateSwapchainKHR(device, &info, allocator, &swapchain));

		CheckVkResult(vkGetSwapchainImagesKHR(device, swapchain, &imageCount, nullptr));
		swapchainImages.resize(imageCount);
		CheckVkResult(vkGetSwapchainImagesKHR(device, swapchain, &imageCount, swapchainImages.data()));

		swapchainImageFormat = surfaceFormat.format;
		swapchainExtent = extent;
    }
}