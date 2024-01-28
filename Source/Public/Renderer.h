#pragma once
#include "window.h"
#include "Device.h"
#include "SwapChain.h"

#include <memory>
#include <cassert>

namespace Application
{
	class Renderer
	{
	public:
		Renderer(Device& device, Window& window);
		~Renderer();

		Renderer(const Renderer&) = delete;
		Renderer& operator=(const Renderer&) = delete;
		
		VkRenderPass GetSwapChainRenderPass() const { return swapChain->GetRenderPass(); }
		bool IsFrameInProgress() const { return isFrameStarted; }
		VkCommandBuffer getCurrentCommandBuffer() const
		{
			assert(isFrameStarted && "Cannot get frame buffer if frame isn't started");
			return commandBuffers[currentImageIndex];
		}

		VkCommandBuffer BeginFrame();
		void EndFrame();
		void BeginSwapChainRenderPass(VkCommandBuffer commandBuffer);
		void EndSwapChainRenderPass(VkCommandBuffer commandBuffer);

	private:
		void CreateCommandBuffers();
		void FreeCommandBuffers();
		void RecreateSwapChain();

		Window& window;
		Device& device;
		std::unique_ptr<SwapChain> swapChain;

		std::vector<VkCommandBuffer> commandBuffers;

		bool isFrameStarted = false;
		uint32_t currentImageIndex;

	};
}