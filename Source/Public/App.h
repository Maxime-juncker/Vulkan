#pragma once

#include "Window.h"
#include "Pipline.h"
#include "Device.h"
#include "SwapChain.h"
#include "Model.h"

#include <memory>
#include <vector>
namespace Application
{
	class App
	{
	public:
		const int MAX_FRAMES_IN_FLIGHT = 2;
		static constexpr int WIDTH = 800;
		static constexpr int HEIGHT = 600;

		App();
		~App();

		App(const App&) = delete;
		App& operator=(const App&) = delete;

		void Run();

	private:
		void CreatePipelineLayout();
		void CreatePipeline();
		void CreateCommandBuffers();
		void CreateSyncObject();
		void DrawFrame();

		void RecordCommandBuffers(VkCommandBuffer commandBuffer, uint32_t imageIndex);

		Window window{ WIDTH, HEIGHT, "Jen fenetre" };
		Device device{ window };
		SwapChain swapChain{ device, window.GetExtend() };
		std::unique_ptr<Pipeline> pipeline;
		Model model{};

		VkPipelineLayout pipelineLayout;
		std::vector<VkCommandBuffer> commandBuffers;

		std::vector<VkSemaphore> imageAvailableSemaphores;
		std::vector<VkSemaphore> renderFinishedSemaphores;
		std::vector<VkFence> inFlightFences;
		std::vector<VkFence> inFlightImages;
		size_t currentFrame = 0;
	
	};

}