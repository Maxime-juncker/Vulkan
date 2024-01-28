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
		Window& operator = (const Window&) = delete;

		bool ShouldClose() { return glfwWindowShouldClose(window); }
		void CreateWindowSurface(VkInstance instance, VkSurfaceKHR* surface);
		bool WasWindowResized() { return frameBufferResized; }
		void ResetWindowResizedFlag() { frameBufferResized = false; }
		VkExtent2D GetExtend() 
		{ 
			return { static_cast<uint32_t>(width), static_cast<uint32_t>(height) }; 
		}
	private:
		static void FrameBufferResizeCallback(GLFWwindow* window, int width, int height);
		void InitWindow();

		int width;
		int height;
		bool frameBufferResized = false;

		std::string windowName;
		GLFWwindow* window;
	};
}