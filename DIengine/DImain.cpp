#include "DImain.h"



int main(int argc, char** argv)
{
	DIEngine::DIApp app;
	try
	{
		DIinitApp();
		app.init();
		app.run();
	}
	catch (std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		return DI_GENERAL_APP_FAILURE;
	}
	return NO_ERROR;
}

void DIEngine::DIApp::init()
{
	initWindow();
	initVulkan();
}

void DIEngine::DIApp::run()
{
	mainLoop();
	cleanup();
}

bool DIEngine::DIApp::checkValidationSupport()
{
	uint32_t layer_count = 0;
	vkEnumerateInstanceLayerProperties(&layer_count, nullptr);

	std::vector<VkLayerProperties> avLayers(layer_count);
	vkEnumerateInstanceLayerProperties(&layer_count, avLayers.data());
	//enumerating validation layers
	for (const char* layer_name : validationLayers)
	{
		bool layersFound = false;
		for (const auto& layer_props : avLayers)
		{
			if (strcmp(layer_name, layer_props.layerName) == 0)
			{
				layersFound = true;
				break;
			}
		}
		if (!layersFound)
		{
			return false;
		}
	}


	return true;
}

void DIEngine::DIApp::setupDebugMessenger()
{
	if (!enableValidation) return;

	VkDebugUtilsMessengerCreateInfoEXT createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
								 VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | 
								 VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | 
								 VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	createInfo.pfnUserCallback = callback_debug;
	createInfo.pUserData = nullptr;

	if (CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS)
	{
		DILog::log(DILog::DILogMessage("Failed to set up debug messenger!", __LINE__, __FILE__, DILog::DI_LOG_LEVEL_ERROR));
	}
}

void DIEngine::DIApp::initWindow()
{
	//Просто тестил логгер. Теперь с цветным выводом!
	//DILog::log(DILog::DILogMessage("test", __LINE__, __FILE__, DILog::DI_LOG_LEVEL_MESSAGE));
	//DILog::log(DILog::DILogMessage("test", __LINE__, __FILE__, DILog::DI_LOG_LEVEL_INFO));
	//DILog::log(DILog::DILogMessage("test", __LINE__, __FILE__, DILog::DI_LOG_LEVEL_WARN));
	//DILog::log(DILog::DILogMessage("test", __LINE__, __FILE__, DILog::DI_LOG_LEVEL_ERROR));
	//DILog::log(DILog::DILogMessage("test", __LINE__, __FILE__, DILog::DI_LOG_LEVEL_FATAL));

	if (DI_APP_SCREEN_TITLE.empty())
	{
		DI_APP_SCREEN_TITLE = std::format("DIengine ver {0}.{1}.{2}", DIENGINE_VER_MAJOR, DIENGINE_VER_MINOR,DIENGINE_VER_PATCH);
		DILog::log(DILog::DILogMessage(std::format("DIengine ver {0}.{1}.{2}", DIENGINE_VER_MAJOR, DIENGINE_VER_MINOR, DIENGINE_VER_PATCH), __LINE__, __FILE__, DILog::DI_LOG_LEVEL_MESSAGE));
	}
	if (DI_APP_NAME.empty())
	{
		DI_APP_NAME = "DIengine project";
	}
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	window = glfwCreateWindow(DI_APP_SCREEN_WIDTH,DI_APP_SCREEN_HEIGHT,DI_APP_SCREEN_TITLE.c_str(), nullptr, nullptr);

}

void DIEngine::DIApp::mainLoop()
{
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();

	}
}

void DIEngine::DIApp::cleanup()
{
	if (enableValidation) {
		DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
	}

	vkDestroyInstance(instance, nullptr);
	glfwDestroyWindow(window);
	glfwTerminate();
}

std::vector<const char*> DIEngine::DIApp::getExtensions()
{
	uint32_t ext_count = 0;
	const char** glfw_ext;
	glfw_ext = glfwGetRequiredInstanceExtensions(&ext_count);

	std::vector<const char*> result(glfw_ext, glfw_ext + ext_count);
	if (enableValidation)
	{
		result.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	}

	return result;
}



void DIEngine::DIApp::initVulkan()
{
	createInstance();
	setupDebugMessenger();
	pickPhysDevice();
	createLogicalDevice();
}

void DIEngine::DIApp::createInstance()
{
	if (enableValidation && !checkValidationSupport())
	{
		DILog::log(DILog::DILogMessage("Validation layers requested, but not found!", __LINE__, __FILE__, DILog::DI_LOG_LEVEL_FATAL));
		throw std::runtime_error("Validation layers error");
	}
	//app info stuff
	VkApplicationInfo appinfo{};
	appinfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appinfo.applicationVersion = VK_MAKE_VERSION(DI_GAME_VER_MAJOR,DI_GAME_VER_MINOR,DI_GAME_VER_PATCH);
	appinfo.pApplicationName = DI_APP_NAME.c_str();
	appinfo.engineVersion = VK_MAKE_VERSION(DIENGINE_VER_MAJOR, DIENGINE_VER_MINOR, DIENGINE_VER_PATCH);
	appinfo.pEngineName = "DIengine";
	appinfo.apiVersion = VK_API_VERSION_1_0;

	VkInstanceCreateInfo createInfo{};
	if (enableValidation) {
		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.data();
	}
	else 
	{
		createInfo.enabledLayerCount = 0;
	}
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appinfo;

	uint32_t glfw_extensions_count = 0;
	const char** glfwExtensions;

	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfw_extensions_count);

	auto glfw_extensions = getExtensions();

	createInfo.enabledExtensionCount = static_cast<uint32_t>(glfw_extensions.size());
	createInfo.ppEnabledExtensionNames = glfw_extensions.data();

	createInfo.enabledLayerCount = 0;

	if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS)
	{
		DILog::log(DILog::DILogMessage("vkCreateInstance failed!", __LINE__, __FILE__, DILog::DI_LOG_LEVEL_FATAL));
		throw std::runtime_error("failed to create instance!");
	}

	uint32_t extCount = 0;
	vkEnumerateInstanceExtensionProperties(nullptr, &extCount, nullptr);

	std::vector<VkExtensionProperties> extensions(extCount);
	vkEnumerateInstanceExtensionProperties(nullptr, &extCount, extensions.data());

	std::vector<std::string> extNames;
	for (const auto& ext : extensions)
	{
		extNames.push_back(ext.extensionName);
	}
	DILog::logVector(DILog::DILogMessage("Vulkan available extensions:", __LINE__, __FILE__), extNames);


}
