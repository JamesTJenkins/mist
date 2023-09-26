#include "renderer/vulkan/VulkanRendererAPI.hpp"
#include <vector>
#include <SDL2/SDL_vulkan.h>
#include "renderer/vulkan/VulkanDebug.hpp"
#include "core/Application.hpp"

namespace mist {
	void VulkanRendererAPI::Initialize() {
		VkResult error;

		// VULKAN INSTANCE
		{
			// APP INFO
			VkApplicationInfo appInfo = {};
			appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
			appInfo.pApplicationName = "Untitled";
			appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
			appInfo.pEngineName = "mist";
			appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
			appInfo.apiVersion = VK_API_VERSION_1_0;

			// EXTENSIONS
#ifdef DEBUG
			std::vector<const char*> extensions = {
				VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
				VK_EXT_DEBUG_REPORT_EXTENSION_NAME
			};
#else
			std::vector<const char*> extensions = {};
#endif

			// GET SDL EXTENSIONS
			unsigned int count = 0;
			SDL_Vulkan_GetInstanceExtensions((SDL_Window*)Application::Get().GetWindow().GetNativeWindow(), &count, nullptr);
			size_t additionalExtensionCount = extensions.size();
			extensions.resize(additionalExtensionCount + count);
			SDL_Vulkan_GetInstanceExtensions((SDL_Window*)Application::Get().GetWindow().GetNativeWindow(), &count, extensions.data() + additionalExtensionCount);
			
			// CREATE INFO
			VkInstanceCreateInfo createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
			createInfo.pApplicationInfo = &appInfo;
			createInfo.ppEnabledExtensionNames = extensions.data();
			createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
			
#ifdef DEBUG
			// VALIDATION LAYERS
			const char* layers[] = {
				"VK_LAYER_KHRONOS_validation"
			};
			createInfo.ppEnabledLayerNames = layers;
			createInfo.enabledLayerCount = 1;
#endif

			error = vkCreateInstance(&createInfo, allocator, &instance);
			CheckVkResult(error);

#ifdef DEBUG
			auto vkCreateDebugReportCallbackEXT = (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugReportCallbackEXT");
			VkDebugReportCallbackCreateInfoEXT debug_report_ci = {};
			debug_report_ci.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
			debug_report_ci.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT | VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT;
			debug_report_ci.pfnCallback = DebugReport;
			debug_report_ci.pUserData = NULL;
			error = vkCreateDebugReportCallbackEXT(instance, &debug_report_ci, allocator, &debugReport);
			CheckVkResult(error);
#endif
		}
		// SELECT GPU
		{
			uint32_t gpuCount;
			error = vkEnumeratePhysicalDevices(instance, &gpuCount, NULL);
			CheckVkResult(error);

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
	}

	void VulkanRendererAPI::Shutdown() {
		vkDestroyInstance(instance, allocator);
	}

	void VulkanRendererAPI::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) {
		
	}

	void VulkanRendererAPI::SetClearColor(glm::vec4& color) {

	}

	void VulkanRendererAPI::Clear() {

	}
}