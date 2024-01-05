#pragma once
#include <vulkan/vulkan.h>
#include <string>
#include <vector>

namespace Application
{
	class Engine
	{
	public:
		Engine();
		~Engine();

	private:
		void InitVulkan();
		void CreateInstance();
		
		void ShowExtensions();
		std::vector<const char*> GetRequiredExtensions();

		VkInstance instance;
	};
}