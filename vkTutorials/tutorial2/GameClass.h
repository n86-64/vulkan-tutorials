/*
*  Vulkan tutorials - Tutorial 2
*  Subject - A basic example drawing a triangle.
*  Date - 29/05/18
*/

#pragma once

// resolution constants. (I know its bad practise but I just want it to work)
#define WIDTH 1920
#define HEIGHT 1080


#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

#include "VulkanUtilities.h"

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

// Vulkan Stuff
private:
	VkInstance			instance = VK_NULL_HANDLE;

	VkPhysicalDevice	physicalDevice = VK_NULL_HANDLE;
	VkDevice			device = VK_NULL_HANDLE;

	// Queues all graphics related commands.
	VkQueue				graphics_queue = VK_NULL_HANDLE;
	VkQueue				present_queue = VK_NULL_HANDLE;

	// Surface/Image we will render to.
	VkSurfaceKHR		render_surface = VK_NULL_HANDLE;

	// Swapchain and swapchain properties info. 
	VkSwapchainKHR		swapchain = VK_NULL_HANDLE;
	VkSurfaceFormatKHR  swapchain_format = {};
	VkPresentModeKHR	swapchain_present_mode = {};
	VkExtent2D			swapchain_image_resolution = {};
	VkFormat			swapchain_image_format = {};
	std::vector<VkImage> swapchain_images;

	std::vector<VkImageView>			swapchain_image_view;

	VkRenderPass		render_pass;
	VkPipelineLayout	pipeline_layout;
	VkPipeline			graphics_pipeline;

	// Vulkan setup functions
	void setupRenderingSurface();
	void selectPhysicalRenderingDevice();
	void createSwapChain();
	void createImageViews();
	void createDevice();
	void createRenderPass();
	void createGraphicsPipeline();

	// Get Vulkan extenstions and add to instance info. 
	void		  getGlfwRequiredVkExtenstions(VkInstanceCreateInfo*  instance_data);
	std::vector<const char*> getRequiredVkExtenstions();

	// vkSwapchain creation functions. (TODO - Add code to perform checks for swapchain setup. Bad practise to not perform checks.)
	void setSwapchainFormat(SwapChainSupportDetails  details);
	void setSwapchainMode(SwapChainSupportDetails  details);
	void setSwapchainViewport(SwapChainSupportDetails  details);

	// Shader Module functions
	void createShaders();
	VkShaderModule createShaderModule(const std::vector<char>& code);

	// Graphics Pipeline Creation


	// Pipeline Creation checks.


	// Shader Modules
	VkShaderModule  vertShaderModule;
	VkShaderModule  fragShaderModule;


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