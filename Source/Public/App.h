#pragma once
#include "window.h"
#include "Pipline.h"
#include "Device.h"
#include "SwapChain.h"
#include "Model.h"

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
		void LoadModels();
		void CreatePipelineLayout();
		void CreatePipeline();
		void CreateCommandBuffers();

		void DrawFrame();
		void RecreateSwapChain();
		void RecordCommandBuffer(int imageIndex);
		void FreeCommandBuffers();

		Window window{ WIDTH, HEIGHT, "Jen fentre" };
		Device device{ window };
		std::unique_ptr<SwapChain> swapChain;
		std::unique_ptr<Pipeline> pipeline;
		std::unique_ptr<Model> model;

		VkPipelineLayout pipelineLayout;
		std::vector<VkCommandBuffer> commandBuffers;

	};
}