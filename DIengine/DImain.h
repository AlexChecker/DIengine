#pragma once

#define DIENGINE_VER_MAJOR 0
#define DIENGINE_VER_MINOR 1
#define DIENGINE_VER_PATCH 0

//Я хочу избавиться от зависимостей к библиотекам в проекте игры
//P.S: Эта идея - полная хуйня
//P.P.S: Видимо не такая уж и хуйня, глобальные переменные все таки нужно отделить
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <format>
#include <string>
#include "ErrorCodes.h"
#include "DILogger.h"

#ifndef DI_GAME_PROJECT
//game version numbers
int DI_GAME_VER_MAJOR = 0;
int DI_GAME_VER_MINOR = 0;
int DI_GAME_VER_PATCH = 0;


int DI_APP_SCREEN_WIDTH  = 500;
int DI_APP_SCREEN_HEIGHT = 500;
std::string DI_APP_SCREEN_TITLE = "";
std::string DI_APP_NAME = "";

#endif
class DIApp 
{
public:
	void init();
	void run();
private:

	VkInstance instance;

	GLFWwindow* window;
	void initWindow();
	void initVulkan();
	void createInstance();
	void mainLoop();
	void cleanup();
};

int DIinitApp();