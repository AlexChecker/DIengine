#define DI_GAME_PROJECT
#include "DImain.h"

//This file used for Vulkan init stuff that can be moved from main cpp file
//and does not depend on variables from header

VkResult DIEngine::DIApp::CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger)
{
	auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
	if (func != nullptr) {
		return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
	}
	else {
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}
}

VKAPI_ATTR VkBool32 VKAPI_CALL DIEngine::DIApp::callback_debug(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData)
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

void DIEngine::DIApp::DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator)
{
	auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
	if (func != nullptr) {
		func(instance, debugMessenger, pAllocator);
	}
}

void DIEngine::DIApp::createLogicalDevice()
{
	familyIndices indices = findQueueFamilies(graphics_card);

	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	std::set<uint32_t> uniqueQueueFamilies = { indices.graphiics_family.value(), indices.present_family.value()};

	float queuePriority = 1.0f;

	for (uint32_t queueFamily : uniqueQueueFamilies)
	{
		VkDeviceQueueCreateInfo queueCreateInfo{};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = queueFamily;
		queueCreateInfo.queueCount = 1;
		queueCreateInfo.pQueuePriorities = &queuePriority;
		queueCreateInfos.push_back(queueCreateInfo);
	}

	VkPhysicalDeviceFeatures deviceFeatures{};

	VkDeviceCreateInfo deviceInfo{};
	deviceInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	deviceInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
	deviceInfo.pQueueCreateInfos = queueCreateInfos.data();
	deviceInfo.queueCreateInfoCount = 1;
	deviceInfo.pEnabledFeatures = &deviceFeatures;
	deviceInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
	deviceInfo.ppEnabledExtensionNames = deviceExtensions.data();

	if (enableValidation)
	{
		deviceInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		deviceInfo.ppEnabledLayerNames = validationLayers.data();
	}
	else
	{
		deviceInfo.enabledLayerCount = 0;
	}


	if (vkCreateDevice(graphics_card, &deviceInfo, nullptr, &device) != VK_SUCCESS)
	{
		DILog::log(DILog::DILogMessage("Failed to create logical device!", __LINE__, __FILE__, DILog::DI_LOG_LEVEL_FATAL));
		throw std::runtime_error("Failed to create logical device!");
	}

	vkGetDeviceQueue(device, indices.graphiics_family.value(), 0, &graphicsQueue);
	vkGetDeviceQueue(device, indices.present_family.value(), 0, &presentQueue);
}
void DIEngine::DIApp::createSurface()
{
	if (glfwCreateWindowSurface(instance, window, nullptr, &surface) != VK_SUCCESS)
	{
		DILog::log(DILog::DILogMessage("Failed to create window surface!", __LINE__, __FILE__, DILog::DI_LOG_LEVEL_FATAL));
		throw std::runtime_error("");
	}
}

DIEngine::familyIndices DIEngine::DIApp::findQueueFamilies(VkPhysicalDevice device)
{
	familyIndices indices;



	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

	std::vector<VkQueueFamilyProperties> props(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, props.data());
	int i = 0;
	for (int i = 0; i < queueFamilyCount; i++)
	{
		VkBool32 presentSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);
		if (presentSupport) indices.present_family = i;
		if (props.at(i).queueFlags & VK_QUEUE_GRAPHICS_BIT)
		{
			indices.graphiics_family = i;
		}
		if (indices.graphiics_family.has_value()) break;
	}

	return indices;
}

/// <summary>
/// Lists and picks up first suitable GPU
/// </summary>
void DIEngine::DIApp::pickPhysDevice()
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

void DIEngine::DIApp::logPhysicalDeviceNames(std::vector<VkPhysicalDevice> devices)
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

bool DIEngine::DIApp::isDeviceSuitable(VkPhysicalDevice device)
{
	familyIndices indices = findQueueFamilies(device);

	VkPhysicalDeviceProperties		deviceProps;
	VkPhysicalDeviceFeatures		deviceFeatures;

	vkGetPhysicalDeviceProperties(device, &deviceProps);
	vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

	bool extesionsupported = checkDeviceExtensionSupport(device);
	
	return deviceProps.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU && deviceFeatures.geometryShader && indices.isComplete() && extesionsupported;
}

bool DIEngine::DIApp::checkDeviceExtensionSupport(VkPhysicalDevice device)
{
	uint32_t extensionCount;
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

	std::vector<VkExtensionProperties> availableExtensions(extensionCount);
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

	std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

	for(const auto& extension : availableExtensions)
	{
		requiredExtensions.erase(extension.extensionName);
    }

	return requiredExtensions.empty();
}
