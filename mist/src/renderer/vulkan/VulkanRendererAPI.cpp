#include "VulkanRendererAPI.hpp"
#include <vulkan/vulkan.h>
#include "VulkanDebug.hpp"

namespace mist {
	void VulkanRendererAPI::Initialize() {
		{
			VkApplicationInfo appInfo = {};
			appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
			appInfo.pApplicationName = "Untitled";
			appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
			appInfo.pEngineName = "mist";
			appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
			appInfo.apiVersion = VK_API_VERSION_1_0;

			VkInstanceCreateInfo createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
			createInfo.pApplicationInfo = &appInfo;
#if DEBUG
			const char* layers[] = { "VK_LAYER_KHRONOS_validation", "VK_EXT_debug_report" };
			createInfo.ppEnabledExtensionNames = layers;
			createInfo.enabledExtensionCount = 2;
#else
			createInfo.ppEnabledExtensionNames = {};
			createInfo.enabledExtensionCount = 0;
#endif
			VkResult error = vkCreateInstance(&createInfo, allocator, &instance);
			check_vk_result(error);
#if DEBUG
			auto vkCreateDebugReportCallbackEXT = (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugReportCallbackEXT");
			VkDebugReportCallbackCreateInfoEXT debug_report_ci = {};
			debug_report_ci.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
			debug_report_ci.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT | VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT;
			debug_report_ci.pfnCallback = debug_report;
			debug_report_ci.pUserData = NULL;
			error = vkCreateDebugReportCallbackEXT(instance, &debug_report_ci, allocator, &debugReport);
			check_vk_result(error);
#endif
		}
	}

	void VulkanRendererAPI::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) {
		
	}

	void VulkanRendererAPI::SetClearColor(glm::vec4& color) {

	}

	void VulkanRendererAPI::Clear() {

	}
}