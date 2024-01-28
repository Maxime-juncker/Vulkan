#include "../Public/App.h"
#include "../Public/RenderSystem.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <stdexcept>
#include <array>

namespace Application
{
	App::App()
	{
		LoadGameObjects();
	}

	App::~App()
	{
	}

	void App::Run()
	{
		RenderSystem renderSystem{device, renderer.GetSwapChainRenderPass()};
		while (!window.ShouldClose())
		{
			glfwPollEvents();
			
			if (auto commandBuffer = renderer.BeginFrame())
			{
				renderer.BeginSwapChainRenderPass(commandBuffer);
				renderSystem.RenderGameObjects(commandBuffer, gameObjects);
				renderer.EndSwapChainRenderPass(commandBuffer);
				renderer.EndFrame();
			}
		}

		// Blocking cpu until gpu finish it's work
		vkDeviceWaitIdle(device.GetDevice());
	}

	void App::LoadGameObjects()
	{
		std::vector<Model::Vertex> vertices
		{
			{{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
			{{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
			{{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}
		};

		auto model = std::make_shared<Model>(device, vertices);
		auto triangle = GameObject::CreateGameObject();
		triangle.model = model;
		triangle.color = { 0.1f, 0.8, 0.1f };
		triangle.transform2d.translation.x = 0.2f;
		triangle.transform2d.rotation = 0.25 * glm::two_pi<float>();

		gameObjects.push_back(std::move(triangle));
	}

}