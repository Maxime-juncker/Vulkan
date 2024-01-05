#include "../Public/App.h"

namespace Application
{
	void App::Run()
	{
		while (!window.ShouldClose())
		{
			glfwPollEvents();
		}
	}
}