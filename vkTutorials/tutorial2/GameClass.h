/*
*  Vulkan tutorials - Tutorial 2
*  Subject - A basic example drawing a triangle.
*  Date - 29/05/18
*/

#pragma once

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

class VKGame 
{
public:
	VKGame() = default;

	// Run the Vulkan program.
	// Returns application exit status.
	int run(); 

private:
	bool init();
	void initVulkan();

	// Add vulkan specific stuff here. 
	void update();
	void cleanup();

private:
	GLFWwindow*   game_window;

	// Vulkan presentation function.
	


// Vulkan Stuff
private:
	VkInstance			instance = VK_NULL_HANDLE;

	VkPhysicalDevice	physicalDevice = VK_NULL_HANDLE;
	VkDevice			device = VK_NULL_HANDLE;

	// Queues all graphics related commands.
	VkQueue				graphics_queue = VK_NULL_HANDLE;
	VkQueue				present_queue = VK_NULL_HANDLE;

	VkSurfaceKHR		render_surface = VK_NULL_HANDLE;


	// Vulkan setup functions
	void setupRenderingSurface();
	void selectPhysicalRenderingDevice();
	void createSwapChain();
	void createDevice();

	// Get Vulkan extenstions and add to instance info. 
	void		  getGlfwRequiredVkExtenstions(VkInstanceCreateInfo*  instance_data);


	// Debugging
	void setupValidationLayers(VkInstanceCreateInfo*   instance_data); 
	void setupDebugCallback();
	VkResult  CreateDebugReportCallbackEXT(VkInstance instance, 
		const VkDebugReportCallbackCreateInfoEXT* pCreateInfo, 
		const VkAllocationCallbacks* pAllocator, 
		VkDebugReportCallbackEXT* pCallback);
	void  DestroyDebugReportCallbackEXT(VkInstance instance,
		VkDebugReportCallbackEXT callback,
		const VkAllocationCallbacks* pAllocator);

	VkDebugReportCallbackEXT	debug_callback_handle;
};