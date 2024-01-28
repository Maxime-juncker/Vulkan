#pragma once
#include "window.h"
#include "Device.h"
#include "Renderer.h"
#include "GameObject.h"

#include <memory>

namespace Application
{
	class App
	{
	public:
		App();
		~App();

		App(const App&) = delete;
		App& operator=(const App&) = delete;

		static constexpr int WIDTH = 800;
		static constexpr int HEIGHT = 600;

		void Run();
	private:
		void LoadGameObjects();

		Window window{ WIDTH, HEIGHT, "Jen fentre" };
		Device device{ window };
		Renderer renderer{ device, window };
		std::vector<GameObject> gameObjects;
	};
}