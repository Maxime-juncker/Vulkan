#include "../Public/Window.h"
#include <GLFW/glfw3.h>
#include <stdexcept>

namespace Application
{
	Window::Window(int w, int h, std::string name) : 
		width{w}, height{h}, windowName{name}
	{
		InitWindow();
	}

	Window::~Window()
	{
		glfwDestroyWindow(window);
		glfwTerminate();
	}

	void Window::InitWindow()
	{
		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

		window = glfwCreateWindow(width, height, windowName.c_str(), nullptr, nullptr);
		glfwSetWindowUserPointer(window, this);
		glfwSetFramebufferSizeCallback(window, FrameBufferResizeCallback);
	}

	void Window::CreateWindowSurface(VkInstance instance, VkSurfaceKHR* surface)
	{
		if (glfwCreateWindowSurface(instance, window, nullptr, surface) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create surface");
		}
	}

	void Window::FrameBufferResizeCallback(GLFWwindow* glfwWindow, int width, int height)
	{
		auto window = reinterpret_cast<Window*>(glfwGetWindowUserPointer(glfwWindow));
		window->frameBufferResized = true;
		window->width = width;
		window->height = height;
	}
}
