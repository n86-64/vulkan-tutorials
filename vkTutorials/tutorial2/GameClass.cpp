#include <iostream>
#include <stdexcept>
#include <vector>


#include "GameClass.h"

// Here all of the global variables for vulkan debugging are stored.
#pragma region VULKAN_GLOBALS


// Names for the validation layers we want to enable.
std::vector<const char*> validationLayers =
{
	"VK_LAYER_LUNARG_standard_validation"
};
#pragma endregion

// Vulkan Debug logger. Logs any errors
static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallbackFunction
(
	VkDebugReportFlagsEXT   flags,
	VkDebugReportObjectTypeEXT objType,
	uint64_t obj,
	size_t location,
	int32_t code,
	const char* layerPrefix,
	const char* msg,
	void* userData
)
{
	std::cerr << "Vulkan validation layer: " << msg << std::endl;
	return VK_FALSE;
}


int VKGame::run()
{
	if (!init()) 
	{
		return EXIT_FAILURE;
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
	getGlfwRequiredVkExtenstions(&app_instance_info);
	setupValidationLayers(&app_instance_info);
	
	if (vkCreateInstance(&app_instance_info, nullptr, &instance) != VK_SUCCESS) 
	{
		throw std::runtime_error("Failed to create the Vulkan Instance");
	}

	setupDebugCallback();
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

#if _DEBUG
	DestroyDebugReportCallbackEXT(instance, debug_callback_handle, nullptr);
#endif

	glfwDestroyWindow(game_window);
	glfwTerminate();
}

// utility functions 

void VKGame::getGlfwRequiredVkExtenstions(VkInstanceCreateInfo* instance_data)
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

// Set up Vulkan validation layers for debug messages and error checking. 
void VKGame::setupValidationLayers(VkInstanceCreateInfo* instance_data)
{
#if _DEBUG
	instance_data->enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
	instance_data->ppEnabledLayerNames = validationLayers.data();
#else 
	instance_data->embledLayerCount = 0;
#endif
}

void VKGame::setupDebugCallback()
{
	VkDebugReportCallbackCreateInfoEXT  debugInfo = {};
	debugInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
	debugInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT;
	debugInfo.pfnCallback = debugCallbackFunction;

	if (CreateDebugReportCallbackEXT(instance, &debugInfo, nullptr, &debug_callback_handle) != VK_SUCCESS)
	{
		std::runtime_error("Debugging layers did not initialise");
	}
}

VkResult VKGame::CreateDebugReportCallbackEXT(VkInstance instance, const VkDebugReportCallbackCreateInfoEXT * pCreateInfo, const VkAllocationCallbacks * pAllocator, VkDebugReportCallbackEXT * pCallback)
{
	auto func = (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugReportCallbackEXT");

	if (func != nullptr) 
	{
		return func(instance, pCreateInfo, pAllocator, pCallback);
	}
	else 
	{
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}
}

void VKGame::DestroyDebugReportCallbackEXT(VkInstance instance, VkDebugReportCallbackEXT callback, const VkAllocationCallbacks * pAllocator)
{
	auto func = (PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugReportCallbackEXT");

	if (func) 
	{
		func(instance, callback, pAllocator);
	}
}
