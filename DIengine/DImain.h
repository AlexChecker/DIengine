#pragma once

#ifndef DI_MAIN_H
#define DI_MAIN_H

#define DIENGINE_VER_MAJOR 0
#define DIENGINE_VER_MINOR 1
#define DIENGINE_VER_PATCH 0

//Windows specific stuff
//TODO: add support for another platforms
#ifdef _WIN32
#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#endif

#include <iostream>
#include "DILogger.h"
#include <optional>
#include <set>

//Я хочу избавиться от зависимостей к библиотекам в проекте игры
//P.S: Эта идея - полная хуйня
//P.P.S: Видимо не такая уж и хуйня, глобальные переменные все таки нужно отделить
#ifndef DI_GAME_PROJECT
#include <stdexcept>
#include <cstdlib>
#include <format>
#include <string>
#include "ErrorCodes.h"
//game version numbers
int DI_GAME_VER_MAJOR = 0;
int DI_GAME_VER_MINOR = 0;
int DI_GAME_VER_PATCH = 0;


int DI_APP_SCREEN_WIDTH  = 500;
int DI_APP_SCREEN_HEIGHT = 500;
std::string DI_APP_SCREEN_TITLE = "";
std::string DI_APP_NAME = "";

#endif
namespace DIEngine {

	typedef struct DI_VK_QUEUE_FAMILY_INDICES
	{
		std::optional<uint32_t> graphiics_family;
		std::optional<uint32_t> present_family;

		bool isComplete()
		{
			return graphiics_family.has_value() && present_family.has_value();
		}
	} familyIndices;

	class DIApp
	{
	public:
		void init();
		void run();
	private:

#ifdef _DEBUG
		bool enableValidation = true;
#else
		bool enableValidation = false;
#endif

		const std::vector<const char*> validationLayers = {
		"VK_LAYER_KHRONOS_validation"
		};

		const std::vector<const char*> deviceExtensions = {
			VK_KHR_SWAPCHAIN_EXTENSION_NAME
		};

		VkInstance instance;
		VkDebugUtilsMessengerEXT debugMessenger;

		///actually, physical device
		VkPhysicalDevice graphics_card = VK_NULL_HANDLE;
		VkDevice device;

		VkQueue graphicsQueue;
		VkQueue presentQueue;

		//Абстрактная хуйня куда все будет рендериться
		VkSurfaceKHR surface;

		GLFWwindow* window;

		bool checkValidationSupport();
		bool isDeviceSuitable(VkPhysicalDevice device);
		bool checkDeviceExtensionSupport(VkPhysicalDevice device);

		void logPhysicalDeviceNames(std::vector<VkPhysicalDevice> devices);

		void setupDebugMessenger();
		void initWindow();
		void initVulkan();
		void createInstance();
		void mainLoop();
		void cleanup();
		void pickPhysDevice();
		void createLogicalDevice();
		void createSurface();

		DI_VK_QUEUE_FAMILY_INDICES findQueueFamilies(VkPhysicalDevice device);

		std::vector<const char*> getExtensions();

		VkResult CreateDebugUtilsMessengerEXT(
			VkInstance instance,
			const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
			const VkAllocationCallbacks* pAllocator,
			VkDebugUtilsMessengerEXT* pDebugMessenger);

		static  VKAPI_ATTR VkBool32 VKAPI_CALL callback_debug(
			VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
			VkDebugUtilsMessageTypeFlagsEXT messageType,
			const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
			void* pUserData);

		static void DestroyDebugUtilsMessengerEXT(
			VkInstance instance,
			VkDebugUtilsMessengerEXT debugMessenger,
			const VkAllocationCallbacks* pAllocator);
	};
}
int DIinitApp();

#endif