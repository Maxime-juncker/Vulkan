#pragma once

#include "Window.h"
#include "Pipline.h"
#include "Engine.h"

namespace Application
{
	class App
	{
	public:

		static constexpr int WIDTH = 800;
		static constexpr int HEIGHT = 600;

		void Run();


	private:
		Window window{WIDTH, HEIGHT, "Jen fenetre"};
		Pipline pipline{ "Ressources/Shaders/SimpleShader.vert.spv", "Ressources/Shaders/SimpleShader.frag.spv" };
		Engine engine;
	};

}