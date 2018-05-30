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

	// Add vulkan specific stuff here. 
	void update();
	void cleanup();

private:
	GLFWwindow* game_window;
};