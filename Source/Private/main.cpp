#include "../Public/App.h"

#include <stdexcept>
#include <cstdlib>
#include <iostream>

int main()
{
	Application::App app{};

	try
	{
		app.Run();
	}
	catch (const std::exception& e)
	{
		std::cout << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}