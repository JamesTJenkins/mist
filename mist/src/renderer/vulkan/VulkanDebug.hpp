#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <vulkan/vulkan.h>
#include <Log.hpp>

static void CheckVkResult(VkResult error) {
	if (error == 0)
		return;
	//fprintf(stderr, "[vulkan] Error: VkResult = %d\n", error);
	MIST_ERROR("[vulkan] Error: VkResult = {0}", error);
	if (error < 0)
		abort();
}

static VKAPI_ATTR VkBool32 VKAPI_CALL DebugUtilsMessengerCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) {
	switch (messageSeverity) {
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:	MIST_ERROR("[vulkan] Debug: {0}", pCallbackData->pMessage); break;
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT: MIST_WARN("[vulkan] Debug: {0}", pCallbackData->pMessage); break;
	default:
		fprintf(stderr, "[vulkan] Debug: %s\n", pCallbackData->pMessage);
	break;
	}
	return VK_FALSE;
}