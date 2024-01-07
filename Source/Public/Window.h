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

		Window(const Window&) = delete;
		Window& operator=(const Window&) = delete;

		bool ShouldClose() { return glfwWindowShouldClose(window); };
		VkExtent2D GetExtend() { return { static_cast<uint32_t>(width), static_cast<uint32_t>(height) }; }
		void CreateWindowSurface(VkInstance instance, VkSurfaceKHR* surface);

		const int width;
		const int height;
	private:

		void InitWindow();
		
		std::string windowName;
		GLFWwindow* window;

	};

}