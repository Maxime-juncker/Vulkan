#include "../Public/Pipline.h"

#include <fstream>
#include <stdexcept>
#include <iostream>

namespace Application
{

	Pipline::Pipline(const std::string& vertFilePath, const std::string& fragFilePath)
	{
		CreateGraphicPipline(vertFilePath, fragFilePath);
	}

	std::vector<char> Pipline::ReadFile(const std::string& path)
	{
		// Opening file
		// Note : std::ios::ate = seek end of file ( | std::ios::binary = read end as binary).	
		std::ifstream file{ path, std::ios::ate | std::ios::binary };

		if (!file.is_open())
		{
			// Could fail because of wrong file path or not having permission to open file.
			throw std::runtime_error("Failed to open file: " + path);
		}

		// Note : file.tellg() = get the current bit (we already are at the end of the file because of std::ios::ate)
		size_t fileSize = static_cast<size_t>(file.tellg());

		// Setting a buffer to the size of the file to hold our data.
		std::vector<char> buffer(fileSize);

		// Go to start of the file
		file.seekg(0);

		// Read the file and put the result in the buffer
		file.read(buffer.data(), fileSize);

		
		file.close();
		return buffer;
	}

	void Pipline::CreateGraphicPipline(const std::string& vertFilePath, const std::string& fragFilePath)
	{
		auto vertCode = ReadFile(vertFilePath);
		auto fragCode = ReadFile(fragFilePath);

		std::cout << "Vertex shader size: " << vertCode.size() << std::endl;
		std::cout << "Fragment shader size: " << fragCode.size() << std::endl;
	}
}
