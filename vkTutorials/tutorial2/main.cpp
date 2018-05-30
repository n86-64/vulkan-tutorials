/*
*  Vulkan tutorials - Tutorial 1 
*  Subject - Initialisation of the Vulkan API. 
*  Date - 29/05/18
*/


#include <iostream>
#include <string>

#include <Windows.h>

#include "vkMath.h"

#include "GameClass.h"


int main(int argc, char* argv[]) 
{
	VKGame   game;
	return game.run();
}