#pragma once

#include "Window.h"
#include "Pipline.h"
#include "Device.h"
#include "SwapChain.h"

namespace Application
{
	class App
	{
	public:

		static constexpr int WIDTH = 800;
		static constexpr int HEIGHT = 600;

		void Run();


	private:
		Window window{ WIDTH, HEIGHT, "Jen fenetre" };
		Device device{ window };
		SwapChain swapChain{ device };
		Pipeline pipleine{ device,
			"Ressources/Shaders/SimpleShader.vert.spv",
			"Ressources/Shaders/SimpleShader.frag.spv",
			Pipeline::DefaultPiplineConfigInfo(WIDTH, HEIGHT)
		};
	};

}