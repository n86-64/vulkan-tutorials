#include <iostream>
#include <stdexcept>


#include "GameClass.h"


int VKGame::run()
{
	if (!init()) 
	{
		return 1;
	}

	update();
	cleanup();

	return EXIT_SUCCESS; // Here is our exit status. 
}

bool VKGame::init()
{
	if (glfwInit() == GLFW_FALSE)
	{
		return false;
	}

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	game_window = glfwCreateWindow(1920, 1080, "vulkan tutorial 2", nullptr, nullptr);
	initVulkan();

	return true;
}

void VKGame::initVulkan()
{
	// Set up our instance and application information.
	VkApplicationInfo app_info = {};
	app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	app_info.pApplicationName = "Vulkan Tutorial 2";
	app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	app_info.pEngineName = "Tutorial Engine";
	app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	app_info.apiVersion = VK_API_VERSION_1_1;

	VkInstanceCreateInfo app_instance_info = {};
	app_instance_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	app_instance_info.pApplicationInfo = &app_info;
	app_instance_info.enabledLayerCount = 0;
	getVkExtenstions(&app_instance_info);
	
	if (vkCreateInstance(&app_instance_info, nullptr, &instance) != VK_SUCCESS) 
	{
		throw std::runtime_error("Failed to create the Vulkan Instance");
	}
}

void VKGame::update()
{
	while (!glfwWindowShouldClose(game_window)) 
	{
		glfwPollEvents();
	}
}

void VKGame::cleanup()
{
	vkDestroyInstance(instance, nullptr); // Cleans up the vulkan session.
	glfwDestroyWindow(game_window);
	glfwTerminate();
}

// utility functions 

void VKGame::getVkExtenstions(VkInstanceCreateInfo* instance_data)
{
	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions;

	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	instance_data->enabledExtensionCount = glfwExtensionCount;
	instance_data->ppEnabledExtensionNames = glfwExtensions;

	std::cout << "GLFW requires: \n";
	for (int i = 0; i < (int)glfwExtensionCount; i++) 
	{
		std::cout << glfwExtensions[i] << std::endl;
	}
}
