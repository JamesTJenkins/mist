#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <vulkan/vulkan.hpp>

static void CheckVkResult(VkResult error) {
    if (error == 0)
    	return;
    fprintf(stderr, "[vulkan] Error: VkResult = %d\n", error);
    if (error < 0)
    	abort();
}

static VKAPI_ATTR VkBool32 VKAPI_CALL DebugReport(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objectType, uint64_t object, size_t location, int32_t messageCode, const char* pLayerPrefix, const char* pMessage, void* pUserData) {
    fprintf(stderr, "[vulkan] Debug report from ObjectType: %i\nMessage: %s\n\n", objectType, pMessage);
	return VK_FALSE;
}