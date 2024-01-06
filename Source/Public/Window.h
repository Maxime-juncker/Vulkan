#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <string>

namespace Application
{
	class Window
	{
	public:
		Window(int w, int h, std::string name);
		~Window();

		bool ShouldClose() { return glfwWindowShouldClose(window); };
		void CreateWindowSurface(VkInstance instance, VkSurfaceKHR* surface);

	private:

		void InitWindow();

		const int width;
		const int height;
		
		std::string windowName;
		GLFWwindow* window;

	};

}