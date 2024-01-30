#include "DImain.h"



int main(int argc, char** argv)
{
	DIApp app;
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

void DIApp::init()
{
	initWindow();
	initVulkan();
}

void DIApp::run()
{
	mainLoop();
	cleanup();
}

void DIApp::initWindow()
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
		DILog::log(DILog::DILogMessage(std::format("DIengine ver {0}.{1}-{2}", DIENGINE_VER_MAJOR, DIENGINE_VER_MINOR, DIENGINE_VER_PATCH), __LINE__, __FILE__, DILog::DI_LOG_LEVEL_MESSAGE));
	}
	if (DI_APP_NAME.empty())
	{
		DI_APP_NAME = "DIengine project";
	}
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	window = glfwCreateWindow(DI_APP_SCREEN_WIDTH,DI_APP_SCREEN_HEIGHT,DI_APP_SCREEN_TITLE.c_str(), nullptr, nullptr);

}

void DIApp::mainLoop()
{
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();

	}
}

void DIApp::cleanup()
{
	glfwDestroyWindow(window);
	glfwTerminate();
}

void DIApp::initVulkan()
{
	createInstance();
}

void DIApp::createInstance()
{
	//app info stuff
	VkApplicationInfo appinfo{};
	appinfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appinfo.applicationVersion = VK_MAKE_VERSION(DI_GAME_VER_MAJOR,DI_GAME_VER_MINOR,DI_GAME_VER_PATCH);
	appinfo.pApplicationName = DI_APP_NAME.c_str();
	appinfo.engineVersion = VK_MAKE_VERSION(DIENGINE_VER_MAJOR, DIENGINE_VER_MINOR, DIENGINE_VER_PATCH);
	appinfo.pEngineName = "DIengine";
	appinfo.apiVersion = VK_API_VERSION_1_0;

	VkInstanceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appinfo;

	uint32_t glfw_extensions_count = 0;
	const char** glfwExtensions;

	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfw_extensions_count);

	createInfo.enabledExtensionCount = glfw_extensions_count;
	createInfo.ppEnabledExtensionNames = glfwExtensions;

	createInfo.enabledLayerCount = 0;

	if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS)
	{
		DILog::log(DILog::DILogMessage("vkCreateInstance failed!", __LINE__, __FILE__, DILog::DI_LOG_LEVEL_FATAL));
		throw std::runtime_error("failed to create instance!");
	}


}
