#pragma once

#include <vector>

#include <vulkan/vulkan.h>

// global aliases

using vkInt = uint32_t;

constexpr int NO_COMPATABLE_QUEUE_FOUND = -1;

struct QueueFamilyIndicies 
{
	int graphicsFamily = NO_COMPATABLE_QUEUE_FOUND;
	int presentFamily = NO_COMPATABLE_QUEUE_FOUND;

	bool isComplete() 
	{
		return graphicsFamily >= 0 && presentFamily >= 0;
	}
};

static QueueFamilyIndicies findCompatableQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR* surface) 
{
	QueueFamilyIndicies  indicies;

	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

	VkBool32 presentSupport = false;

	for (int i = 0; i < queueFamilyCount; i++) 
	{
		if (queueFamilies[i].queueCount > 0 && (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)) 
		{
			indicies.graphicsFamily = i;
		}

		if (!presentSupport) 
		{
			vkGetPhysicalDeviceSurfaceSupportKHR(device, i, *surface, &presentSupport);
			if (presentSupport) 
			{
				indicies.presentFamily = i;
			}
		}

		if (indicies.isComplete()) 
		{
			break;
		}
	}

	return indicies;
}


// Swapchain setup functions

// Swapchain support information.
struct SwapChainSupportDetails 
{
	VkSurfaceCapabilitiesKHR	   capibilities;
	std::vector<VkSurfaceFormatKHR>		formats;
	std::vector<VkPresentModeKHR>  presentModes;
};

// Retrieve information about the swap chain support. 
static SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR* surface) 
{
	SwapChainSupportDetails  details;

	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, *surface, &details.capibilities);

	uint32_t formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(device, *surface, &formatCount, nullptr);
	if (formatCount != 0) 
	{
		details.formats.resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, *surface, &formatCount, details.formats.data());
	}

	uint32_t presentModeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(device, *surface, &presentModeCount, nullptr);
	if (presentModeCount != 0)
	{
		details.presentModes.resize(presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, *surface, &presentModeCount, details.presentModes.data());
	}

	return details;
}

