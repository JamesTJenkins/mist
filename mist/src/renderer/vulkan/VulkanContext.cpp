#include "VulkanContext.hpp"
#include <glm/glm.hpp>
#include <set>
#include <SDL3/SDL_vulkan.h>
#include "renderer/vulkan/VulkanDebug.hpp"
#include "Application.hpp"
#include "Debug.hpp"

namespace mist {
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
		
		std::vector<const char*> extensions = {};
		unsigned int count = 0;
		const char* const * sdlExtensions = SDL_Vulkan_GetInstanceExtensions(&count);
		extensions.reserve(count);
		for (size_t i = 0; i < count; i++) {
			extensions.emplace_back(sdlExtensions[i]);
		}

#ifdef DEBUG
		extensions.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif

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

		CheckVkResult(vkCreateDebugUtilsMessengerEXT(instance, &debugUtilsMessengerCI, allocator, &debugMessenger));
#endif
	}

	void VulkanContext::CreateSurface() {
		if (!SDL_Vulkan_CreateSurface(Application::Get().GetWindow()->GetNativeWindow(), instance, allocator, &surface))
			printf("[VULKAN] surface failed to be created");
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
		CheckVkResult(vkCreateDevice(physicalDevice, &deviceCreateInfo, allocator, &device));
		vkGetDeviceQueue(device, indicies.graphicsFamily.value(), 0, &graphicsQueue);
		vkGetDeviceQueue(device, indicies.presentFamily.value(), 0, &presentQueue);
	}

	void VulkanContext::CreateSwapchain(FramebufferProperties& properties) {
		swapchain = CreateRef<VulkanSwapchain>();
		swapchain->CreateSwapchain(properties);
	}
}