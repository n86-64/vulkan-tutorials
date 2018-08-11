#include <iostream>
#include <stdexcept>
#include <vector>
#include <set>

#include "FileIO.h"



#include "GameClass.h"


// Here all of the global variables for vulkan debugging are stored.
#pragma region VULKAN_GLOBALS


// Names for the validation layers we want to enable.
std::vector<const char*> validationLayers =
{
	"VK_LAYER_LUNARG_standard_validation"
};
#pragma endregion

// Here we store the series of extenstions that we need to enable for making games. 
// This inclues core components such as the swapchain. 
#pragma region VULKAN_EXTENSTIONS
std::vector<const char*> device_extenstions
{
	VK_KHR_SWAPCHAIN_EXTENSION_NAME
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

	game_window = glfwCreateWindow(WIDTH, HEIGHT, "vulkan tutorial 2", nullptr, nullptr);
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
	// getGlfwRequiredVkExtenstions(&app_instance_info);

	auto extenstions = getRequiredVkExtenstions();
	app_instance_info.enabledExtensionCount = static_cast<vkInt>(extenstions.size());
	app_instance_info.ppEnabledExtensionNames = extenstions.data();

	setupValidationLayers(&app_instance_info);

	if (vkCreateInstance(&app_instance_info, nullptr, &instance) != VK_SUCCESS) 
	{
		throw std::runtime_error("Failed to create the Vulkan Instance");
	}

	setupDebugCallback();
	setupRenderingSurface();
	selectPhysicalRenderingDevice();
	createDevice();
	createSwapChain();
	createRenderPass();
	createGraphicsPipeline();
}

void VKGame::update()
{
	while (!glfwWindowShouldClose(game_window)) 
	{
		glfwPollEvents();

		int state = glfwGetKey(game_window, GLFW_KEY_ESCAPE);
		
		// Kill the loop.
		if (state == GLFW_PRESS) 
		{
			glfwSetWindowShouldClose(game_window, true);
		}
	}
	
}

void VKGame::cleanup()
{
	for (auto imageView : swapchain_image_view) 
	{
		vkDestroyImageView(device, imageView, nullptr);
	}

	vkDestroyShaderModule(device, vertShaderModule, nullptr);
	vkDestroyShaderModule(device, fragShaderModule, nullptr);

	vkDestroyPipeline(device, graphics_pipeline, nullptr);
	vkDestroyPipelineLayout(device, pipeline_layout, nullptr);
	vkDestroyRenderPass(device, render_pass, nullptr);

	vkDestroySwapchainKHR(device, swapchain, nullptr); 
	vkDestroyDevice(device, nullptr);
	vkDestroySurfaceKHR(instance, render_surface, nullptr); // Destroy the render surface.
	vkDestroyInstance(instance, nullptr); // Cleans up the vulkan session.

#if _DEBUG
	DestroyDebugReportCallbackEXT(instance, debug_callback_handle, nullptr);
#endif

	glfwDestroyWindow(game_window);
	glfwTerminate();
}

// utility functions 

void VKGame::setupRenderingSurface()
{
	if (glfwCreateWindowSurface(instance, game_window, nullptr, &render_surface) != VK_SUCCESS) 
	{
		throw std::runtime_error("failed to create a window surface.");
	}
}

// Here we select the GPU that we want to use and then proceed to utalise it for rendering.
// We will use this to create our queues which in turn are tied to our command pools.
void VKGame::selectPhysicalRenderingDevice()
{
	uint32_t  deviceCount = 0;
	vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

	std::vector<VkPhysicalDevice> devices(deviceCount);
	vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

	if (deviceCount == 0)
	{
		throw std::runtime_error("Failed to find GPU with Vulkan Support!");
	}

	// Just select the first GPU/Device for now. 
	//In future checks should be implimented when selecting devices which are then used for setup to ensure the desired queue families are supported by the GPU.
	physicalDevice = devices[0];
}

// Decide the swap chain settings and setup the swapchain mechanism. 
void VKGame::createSwapChain()
{
	SwapChainSupportDetails  swapchain_details = querySwapChainSupport(physicalDevice, &render_surface);

	// WARNING - Do not do this. Formats should be checked when setting up the swapchain. 
	setSwapchainFormat(swapchain_details);
	setSwapchainMode(swapchain_details);
	setSwapchainViewport(swapchain_details);

	// Now our data values are set we can setup the swapchain. 

	vkInt imageCount = swapchain_details.capibilities.minImageCount + 1;
	if (swapchain_details.capibilities.maxImageCount > 0 && imageCount > swapchain_details.capibilities.maxImageCount) 
	{
		imageCount = swapchain_details.capibilities.maxImageCount;
	}

	VkSwapchainCreateInfoKHR swapchain_setup_info = {};
	swapchain_setup_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	swapchain_setup_info.surface = render_surface;
	swapchain_setup_info.minImageCount = imageCount;
	swapchain_setup_info.imageFormat = swapchain_format.format;
	swapchain_setup_info.imageColorSpace = swapchain_format.colorSpace;
	swapchain_setup_info.imageExtent = swapchain_image_resolution;
	swapchain_setup_info.imageArrayLayers = 1;
	swapchain_setup_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	QueueFamilyIndicies  indicies = findCompatableQueueFamilies(physicalDevice, &render_surface);
	vkInt queueIndicies[] = { (vkInt)indicies.graphicsFamily, (vkInt)indicies.presentFamily };

	if (indicies.graphicsFamily != indicies.presentFamily) 
	{
		swapchain_setup_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		swapchain_setup_info.queueFamilyIndexCount = 2;
		swapchain_setup_info.pQueueFamilyIndices = queueIndicies;
	}
	else 
	{
		swapchain_setup_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	}

	swapchain_setup_info.preTransform = swapchain_details.capibilities.currentTransform;
	swapchain_setup_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	swapchain_setup_info.presentMode = swapchain_present_mode;
	swapchain_setup_info.clipped = VK_TRUE;
	swapchain_setup_info.oldSwapchain = VK_NULL_HANDLE;

	if (vkCreateSwapchainKHR(device, &swapchain_setup_info, nullptr, &swapchain) != VK_SUCCESS) 
	{
		std::runtime_error("Error - Swapchian creation failed");
	}

	vkGetSwapchainImagesKHR(device, swapchain, &imageCount, nullptr);
	swapchain_images.resize(imageCount);
	vkGetSwapchainImagesKHR(device, swapchain, &imageCount, swapchain_images.data());

	createImageViews(); // Create Samplers for the render targets.
}

void VKGame::createImageViews()
{
	swapchain_image_view.resize(swapchain_images.size());

	for (size_t i = 0; i < swapchain_image_view.size(); i++) 
	{
		VkImageViewCreateInfo image_view_setup = {};
		image_view_setup.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		image_view_setup.image = swapchain_images[i];
		image_view_setup.viewType = VK_IMAGE_VIEW_TYPE_2D;
		image_view_setup.format = swapchain_image_format;

		image_view_setup.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		image_view_setup.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
	    image_view_setup.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		image_view_setup.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

		image_view_setup.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		image_view_setup.subresourceRange.baseMipLevel = 0;
		image_view_setup.subresourceRange.levelCount = 1;
		image_view_setup.subresourceRange.baseArrayLayer = 0;
		image_view_setup.subresourceRange.layerCount = 1;

		if (vkCreateImageView(device, &image_view_setup, nullptr, &swapchain_image_view[i]) != VK_SUCCESS) 
		{
			throw std::runtime_error("failed to create image views!");
		}
	}
}

void VKGame::createDevice()
{
	QueueFamilyIndicies compatable_queue_indicies = findCompatableQueueFamilies(physicalDevice, &render_surface);

	if (!compatable_queue_indicies.isComplete())
	{
		throw std::runtime_error("Failed to find GPU with Vulkan Support");
	}

	std::vector<VkDeviceQueueCreateInfo>  queueCreationObjects;
	std::set<int>  uniqueQueueFamilies = { compatable_queue_indicies.graphicsFamily,  compatable_queue_indicies.presentFamily };


	// Setup the multiple queues for performing graphics operations and drawing.
	float queuePriority = 1.0f;
	for (int queueFamily : uniqueQueueFamilies) 
	{
		VkDeviceQueueCreateInfo     queueCreateInfo = {};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = queueFamily;
		queueCreateInfo.queueCount = 1;
		queueCreateInfo.pQueuePriorities = &queuePriority;
		queueCreationObjects.push_back(queueCreateInfo);
	}

	// Setup the device object which is what we use to render the queues.
	
	// Get Device Features
	VkPhysicalDeviceFeatures deviceFeatures = {};

	VkDeviceCreateInfo   device_info = {};
	device_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	device_info.queueCreateInfoCount = static_cast<uint32_t>(queueCreationObjects.size());
	device_info.pQueueCreateInfos = queueCreationObjects.data();
	device_info.pEnabledFeatures = &deviceFeatures;
	device_info.enabledExtensionCount = static_cast<uint32_t>(device_extenstions.size());
	device_info.ppEnabledExtensionNames = device_extenstions.data();

#if _DEBUG
	device_info.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
	device_info.ppEnabledLayerNames = validationLayers.data();
#else 
	device_info.embledLayerCount = 0;
#endif

	if (vkCreateDevice(physicalDevice, &device_info, nullptr, &device) != VK_SUCCESS) 
	{
		throw std::runtime_error("failed to create logical device!");
	}

	vkGetDeviceQueue(device, compatable_queue_indicies.graphicsFamily, 0, &graphics_queue);
	vkGetDeviceQueue(device, compatable_queue_indicies.presentFamily, 0, &present_queue);
}

void VKGame::createRenderPass()
{
	// Attachments to passes represent data to pipe into the shaders.
	// In this case the vertex colors.
	VkAttachmentDescription colorAttachment = {};
	colorAttachment.format = swapchain_image_format;
	colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;

	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

	colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	VkAttachmentReference colorAttachmentRef = {};
	colorAttachmentRef.attachment = 0;
	colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpass = {};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colorAttachmentRef;

	VkRenderPassCreateInfo renderPassInfo = {};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount = 1;
	renderPassInfo.pAttachments = &colorAttachment;
	renderPassInfo.subpassCount = 1;
	renderPassInfo.pSubpasses = &subpass;

	if (vkCreateRenderPass(device, &renderPassInfo, nullptr, &render_pass) != VK_SUCCESS) 
	{
		throw std::runtime_error("failed to create render pass!!");
	}

}

// TODO - Remove this function.
void VKGame::getGlfwRequiredVkExtenstions(VkInstanceCreateInfo* instance_data)
{
	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions;

	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

#ifndef _DEBUG
	std::vector<const char*> extenstions(glfwExtensions, glfwExtensions + glfwExtensionCount);
	extenstions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
	instance_data->enabledExtensionCount = static_cast<vkInt>(extenstions.size());
	instance_data->ppEnabledExtensionNames = extenstions.data();
#else
	instance_data->enabledExtensionCount = glfwExtensionCount;
	instance_data->ppEnabledExtensionNames = glfwExtensions;
#endif

	std::cout << "GLFW requires: \n";
	for (int i = 0; i < (int)glfwExtensionCount; i++) 
	{
		std::cout << glfwExtensions[i] << std::endl;
	}
}

std::vector<const char*> VKGame::getRequiredVkExtenstions()
{
	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions;

	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	std::vector<const char*> extenstions(glfwExtensions, glfwExtensions + glfwExtensionCount);

#ifdef _DEBUG
	extenstions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
#endif

	return extenstions;
}

void VKGame::setSwapchainFormat(SwapChainSupportDetails details)
{
	swapchain_format = { VK_FORMAT_R8G8B8A8_UNORM, VK_COLORSPACE_SRGB_NONLINEAR_KHR };
	swapchain_image_format = swapchain_format.format;
}

void VKGame::setSwapchainMode(SwapChainSupportDetails details)
{
	for (const auto& render_mode : details.presentModes) 
	{ 
		if (render_mode == VK_PRESENT_MODE_MAILBOX_KHR) 
		{
			swapchain_present_mode = render_mode;
			return;
		}
		else if (render_mode == VK_PRESENT_MODE_FIFO_RELAXED_KHR)
		{
			swapchain_present_mode = render_mode;
			return;
		}
	}

	swapchain_present_mode = VK_PRESENT_MODE_FIFO_KHR; // Once an image is rendered push to the front buffer and then render immediatly with no time restrictions.
}

void VKGame::setSwapchainViewport(SwapChainSupportDetails details)
{
	swapchain_image_resolution = { 1920, 1080 };
}

void VKGame::createShaders()
{
	auto vertShaderCode = readFile("vert.spv");
	auto fragShaderCode = readFile("frag.spv");

	vertShaderModule = createShaderModule(vertShaderCode);
	fragShaderModule = createShaderModule(fragShaderCode);
}

VkShaderModule VKGame::createShaderModule(const std::vector<char>& code)
{
	VkShaderModule shaderModule;

	VkShaderModuleCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = code.size();
	createInfo.pCode = reinterpret_cast<const vkInt*>(code.data());

	if (vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) 
	{
		throw std::runtime_error("failed to create shader module!");
	}


	return shaderModule;
}

// Create the rendering pipeline which will recieve input and then output images accordignly.
void VKGame::createGraphicsPipeline()
{
	createShaders();

	VkPipelineShaderStageCreateInfo  vertShaderStageCreateInfo = {};
	vertShaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vertShaderStageCreateInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
	vertShaderStageCreateInfo.module = vertShaderModule;
	vertShaderStageCreateInfo.pName = "main";

	VkPipelineShaderStageCreateInfo  fragShaderStageCreateInfo = {};
	fragShaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	fragShaderStageCreateInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	fragShaderStageCreateInfo.module = fragShaderModule;
	fragShaderStageCreateInfo.pName = "main";

	VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageCreateInfo, fragShaderStageCreateInfo };

	// Create pipeline elements setting up the 
	VkPipelineVertexInputStateCreateInfo  vertexInputInfo = {};
	vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInputInfo.vertexBindingDescriptionCount = 0;
	vertexInputInfo.vertexAttributeDescriptionCount = 0;

	VkPipelineInputAssemblyStateCreateInfo inputAssembly = {}; 
	inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	inputAssembly.primitiveRestartEnable = VK_FALSE;


	VkViewport viewport = {};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = (float)swapchain_image_resolution.width;
	viewport.height = (float)swapchain_image_resolution.height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	VkRect2D scissor = {};
	scissor.offset = { 0, 0 };
	scissor.extent = swapchain_image_resolution;

	VkPipelineViewportStateCreateInfo viewportState;
	viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportState.viewportCount = 1;
	viewportState.pViewports = &viewport;
	viewportState.scissorCount = 1;
	viewportState.pScissors = &scissor;

	VkPipelineRasterizationStateCreateInfo rasterizer = {};
	rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizer.depthClampEnable = VK_FALSE;
	rasterizer.rasterizerDiscardEnable = VK_FALSE;
	rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
	rasterizer.lineWidth = 1.0f;
	rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
	rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
	rasterizer.depthBiasEnable = VK_FALSE;

	VkPipelineMultisampleStateCreateInfo multisampler = {};
	multisampler.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampler.sampleShadingEnable = VK_FALSE;
	multisampler.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

	VkPipelineColorBlendAttachmentState blendConfiguration;
	blendConfiguration.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	blendConfiguration.blendEnable = VK_FALSE;

	VkPipelineColorBlendStateCreateInfo blendState = {};
	blendState.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	blendState.logicOpEnable = VK_FALSE;
	blendState.attachmentCount = 1;
	blendState.pAttachments = &blendConfiguration;

	VkDynamicState dynamicStates[] = {
		VK_DYNAMIC_STATE_VIEWPORT,
		VK_DYNAMIC_STATE_LINE_WIDTH
	};

	VkPipelineDynamicStateCreateInfo dynamicState = {};
	dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamicState.dynamicStateCount = 2;
	dynamicState.pDynamicStates = dynamicStates;

	VkPipelineLayoutCreateInfo  pipelineLayoutInfo = {};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;

	if (vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &pipeline_layout) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create pipeline layout!");
	}

	VkGraphicsPipelineCreateInfo pipelineCreateInfo = {};
	pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;

	pipelineCreateInfo.stageCount = 2;
	pipelineCreateInfo.pStages = shaderStages;

	pipelineCreateInfo.pVertexInputState = &vertexInputInfo;
	pipelineCreateInfo.pInputAssemblyState = &inputAssembly;
	pipelineCreateInfo.pViewportState = &viewportState;
	pipelineCreateInfo.pRasterizationState = &rasterizer;
	pipelineCreateInfo.pMultisampleState = &multisampler;
	pipelineCreateInfo.pColorBlendState = &blendState;

	pipelineCreateInfo.layout = pipeline_layout;
	pipelineCreateInfo.renderPass = render_pass;
	pipelineCreateInfo.subpass = 0;

	if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineCreateInfo, nullptr, &graphics_pipeline) != VK_SUCCESS) 
	{
		throw std::runtime_error("failed to create graphics pipeline!");
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
