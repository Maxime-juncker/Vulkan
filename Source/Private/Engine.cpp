#include "../Public/Engine.h"

#define GLFW_INLCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <stdexcept>
#include <iostream>
#include <vector>
#include <unordered_set>
#include <string>

namespace Application
{
	Engine::Engine()
	{
		InitVulkan();
	}

	Engine::~Engine()
	{
		vkDestroyInstance(instance, nullptr);
	}

	void Engine::InitVulkan()
	{
		CreateInstance();
		ShowExtensions();
	}

	void Engine::CreateInstance()
	{
		// Setting app infos.
		VkApplicationInfo appInfo{};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName = "Jen Application";
		appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.pEngineName = "No Engine";
		appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.apiVersion = VK_API_VERSION_1_0;

		// Setting the instance infos
		VkInstanceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		createInfo.pApplicationInfo = &appInfo;

		uint32_t glfwExtensions = 0;
		const char** glfwExtensionCount;

		glfwExtensionCount = glfwGetRequiredInstanceExtensions(&glfwExtensions);

		createInfo.enabledExtensionCount = glfwExtensions;
		createInfo.ppEnabledExtensionNames = glfwExtensionCount;



		createInfo.enabledLayerCount = 0;

		// Creating vulkan instance
		if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create instance!");
		}

	}

	void Engine::ShowExtensions()
	{
		uint32_t extensionCount;
		vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
		std::vector<VkExtensionProperties> extensions(extensionCount);

		vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());
		std::unordered_set<std::string> available;
		
		std::cout << "\nAvalaible Extension : " << std::endl;
		for (const auto& extension : extensions)
		{
			std::cout << '\t' << extension.extensionName << std::endl;
			available.insert(extension.extensionName);

		}

		std::cout << "\nRequired extensions:" << std::endl;
		auto requiredExtensions = GetRequiredExtensions();
		for (const auto& extension : requiredExtensions)
		{
			std::cout << "\t" << extension << std::endl;
			if (available.find(extension) == available.end())
			{
				throw std::runtime_error("Missing required extension");
			}
		}

	}

	std::vector<const char*> Engine::GetRequiredExtensions()
	{
		uint32_t glfwExtensionCount = 0;
		const char** glfwExtensions;
		glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
		
		std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

		return extensions;
		return std::vector<const char*>();
	}



}
