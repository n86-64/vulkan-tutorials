/*
*  Vulkan tutorials - Tutorial 1 
*  Subject - Initialisation of the Vulkan API. 
*  Date - 29/05/18
*/


#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <string>

#include <Windows.h>


void cleanup() 
{
	glfwTerminate();
}


int main(int argc, char* argv[]) 
{
	if (glfwInit() == GLFW_FALSE) 
	{
		MessageBoxW(nullptr, L"GLFW failed to initalise.", L"vkCriticalError", MB_ICONERROR | MB_OK);
		return 1;
	}

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	GLFWwindow*   window = glfwCreateWindow(1920, 1080, "vulkan tutorial 1", nullptr, nullptr);

	uint32_t   extenstionCount = 0;
	vkEnumerateInstanceExtensionProperties(nullptr, &extenstionCount, nullptr);

	std::cout << "count - " << extenstionCount;
	
	while (!glfwWindowShouldClose(window)) 
	{
		glfwPollEvents();
	}

	glfwDestroyWindow(window);
	cleanup();

	return 0;
}