#pragma once

// Utilities will be quick and dirty c-style functions as 
// C++ class not needed.

#include <vector>
#include <string>
#include <fstream>

std::vector<char> readFile(const std::string filename) 
{
	std::fstream  file(filename, std::ios::in | std::ios::ate | std::ios::binary);

	if (file.fail()) 
	{
		std::runtime_error("File failed to open");
	}

	size_t fileSize = (size_t)file.tellg();
	std::vector<char> buffer(fileSize);

	file.seekg(0);
	file.read(buffer.data(), fileSize);

	file.close();

	return buffer;
}