#include "GameClass.h"

constexpr int EXIT_SUCCESS = 0;
constexpr int EXIT_FALIURE = 1;

int VKGame::run()
{
	if (!init()) 
	{
		return EXIT_FALIURE;
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

	game_window = glfwCreateWindow(1920, 1080, "vulkan tutorial 1", nullptr, nullptr);


	return true;
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
	glfwDestroyWindow(game_window);
	glfwTerminate();
}
