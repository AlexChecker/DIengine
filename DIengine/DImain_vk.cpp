#define DI_GAME_PROJECT
#include "DImain.h"

//This file used for Vulkan init stuff that can be moved from main cpp file
//and does not depend on variables from header

VkResult DIApp::CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger)
{
	auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
	if (func != nullptr) {
		return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
	}
	else {
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}
}

VKAPI_ATTR VkBool32 VKAPI_CALL DIApp::callback_debug(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData)
{
	switch (messageSeverity)
	{
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
		DILog::log(DILog::DILogMessage(pCallbackData->pMessage, 0, "Vulkan debug diagnostic message", DILog::DI_LOG_LEVEL_MESSAGE));
		break;
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
		DILog::log(DILog::DILogMessage(pCallbackData->pMessage, 0, "Vulkan debug info message", DILog::DI_LOG_LEVEL_INFO));
		break;
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
		DILog::log(DILog::DILogMessage(pCallbackData->pMessage, 0, "Vulkan debug warning message", DILog::DI_LOG_LEVEL_WARN));
		break;
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
		DILog::log(DILog::DILogMessage(pCallbackData->pMessage, 0, "Vulkan debug error message", DILog::DI_LOG_LEVEL_ERROR));
		break;
	}
	return VK_FALSE;
}

void DIApp::DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator)
{
	auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
	if (func != nullptr) {
		func(instance, debugMessenger, pAllocator);
	}
}

/// <summary>
/// Lists and picks up first suitable GPU
/// </summary>

void DIApp::pickPhysDevice()
{
	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
	if (deviceCount == 0)
	{
		DILog::log(DILog::DILogMessage("Graphics cards not found!", __LINE__, __FILE__, DILog::DI_LOG_LEVEL_FATAL));
		throw std::runtime_error("Graphics cards not found!");
	}
	std::vector<VkPhysicalDevice> devices(deviceCount);
	vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

	logPhysicalDeviceNames(devices);

	for (const auto& device : devices)
	{
		if (isDeviceSuitable(device))
		{
			graphics_card = device;
			break;
		}

	}

	if (graphics_card == VK_NULL_HANDLE)
	{

		DILog::log(DILog::DILogMessage("Suitable graphics cards not found!", __LINE__, __FILE__, DILog::DI_LOG_LEVEL_FATAL));
		throw std::runtime_error("Suitable graphics cards not found!");
	}


	
}

void DIApp::logPhysicalDeviceNames(std::vector<VkPhysicalDevice> devices)
{
	std::vector<std::string> deviceNames;
	
	for (const auto& device : devices)
	{
		VkPhysicalDeviceProperties deviceProps;
		vkGetPhysicalDeviceProperties(device, &deviceProps);
		deviceNames.push_back(deviceProps.deviceName);
	}
	DILog::logVector(DILog::DILogMessage("Graphics cards found: ", __LINE__, __FILE__, DILog::DI_LOG_LEVEL_MESSAGE), deviceNames);
}

bool DIApp::isDeviceSuitable(VkPhysicalDevice device)
{
	VkPhysicalDeviceProperties		deviceProps;
	VkPhysicalDeviceFeatures		deviceFeatures;

	vkGetPhysicalDeviceProperties(device, &deviceProps);
	vkGetPhysicalDeviceFeatures(device, &deviceFeatures);
	
	return deviceProps.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU && deviceFeatures.geometryShader;
}

