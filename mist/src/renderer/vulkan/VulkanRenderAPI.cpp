#include "VulkanRenderAPI.hpp"
#include "renderer/vulkan/VulkanContext.hpp"
#include <Log.hpp>

namespace mist {
	void VulkanRenderAPI::Initialize() {
		VulkanContext& context = VulkanContext::GetContext();

		context.CreateInstance();
		context.CreateSurface();
		context.CreatePhysicalDevice();
		context.CreateDevice();
	}

	void VulkanRenderAPI::Shutdown() {
		VulkanContext& context = VulkanContext::GetContext();

		vkDestroyDevice(context.GetDevice(), context.GetAllocationCallbacks());
		vkDestroySurfaceKHR(context.GetInstance(), context.GetSurface(), context.GetAllocationCallbacks());
#ifdef DEBUG
		auto vkDestroyDebugReportCallbackEXT = (PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr(context.GetInstance(), "vkDestroyDebugReportCallbackEXT");
		vkDestroyDebugReportCallbackEXT(context.GetInstance(), context.GetDebugCallback(), context.GetAllocationCallbacks());
#endif
		vkDestroyInstance(context.GetInstance(), context.GetAllocationCallbacks());
	}

	void VulkanRenderAPI::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) {
		
	}

	void VulkanRenderAPI::SetClearColor(glm::vec4& color) {

	}

	void VulkanRenderAPI::Clear() {

	}
}