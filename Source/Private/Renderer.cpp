#include "../Public/Renderer.h"

#include <stdexcept>
#include <array>

namespace Application
{

	Renderer::Renderer(Device& device, Window& window) : device{device}, window{window}
	{
		RecreateSwapChain();
		CreateCommandBuffers();
	}

	Renderer::~Renderer()
	{
		FreeCommandBuffers();
	}

	void Renderer::CreateCommandBuffers()
	{
		// Creating command buffers
		commandBuffers.resize(SwapChain::MAX_FRAMES_IN_FLIGHT);

		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = device.GetCommandPool();
		allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

		if (vkAllocateCommandBuffers(device.GetDevice(), &allocInfo, commandBuffers.data()) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to allocates command buffers");
		}
	}

	void Renderer::RecreateSwapChain()
	{
		auto extend = window.GetExtend();
		// User minimized the window
		while (extend.width = +0 || extend.height == 0)
		{
			extend = window.GetExtend();
			glfwWaitEvents();
		}
		// Waiting for swapchain to not be in use
		vkDeviceWaitIdle(device.GetDevice());
		// Recreating the swapchain
		if (swapChain == nullptr)
		{
			swapChain = std::make_unique<SwapChain>(device, extend);
		}
		else
		{
			std::shared_ptr<SwapChain> oldSwapChain = std::move(swapChain);
			swapChain = std::make_unique<SwapChain>(device, extend, oldSwapChain);

			if (!oldSwapChain->CompareSwapFormats(*swapChain.get()))
			{
				throw std::runtime_error("Swap chain image or depth format has change");
			}
		}
	}

	void Renderer::FreeCommandBuffers()
	{
		vkFreeCommandBuffers
		(
			device.GetDevice(),
			device.GetCommandPool(),
			static_cast<uint32_t>(commandBuffers.size()),
			commandBuffers.data()
		);
		commandBuffers.clear();
	}

	VkCommandBuffer Renderer::BeginFrame()
	{
		assert(!isFrameStarted && "Can't start new frame while already making an other");
		auto result = swapChain->AcquireNextImage(&currentImageIndex);

		// Window probably resized
		if (result == VK_ERROR_OUT_OF_DATE_KHR)
		{
			RecreateSwapChain();
			return nullptr;
		}

		if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
		{
			throw std::runtime_error("Failed to acquire swap chain image");
		}

		isFrameStarted = true;

		auto commandBuffer = getCurrentCommandBuffer();
		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to start to record command buffer");
		}

		return commandBuffer;
	}

	void Renderer::EndFrame()
	{
		assert(isFrameStarted && "Can't end frame if it didn't begin");
		auto commandBuffer = getCurrentCommandBuffer();
		if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to record buffer");
		}

		auto result = swapChain->SubmitCommandBuffers(&commandBuffer, &currentImageIndex);
		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR ||
			window.WasWindowResized())
		{
			window.ResetWindowResizedFlag();
			RecreateSwapChain();
		}
		else if (result != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to present swap chain image");
		}

		isFrameStarted = false;
		currentFrameIndex = (currentFrameIndex + 1) % SwapChain::MAX_FRAMES_IN_FLIGHT;
	}

	void Renderer::BeginSwapChainRenderPass(VkCommandBuffer commandBuffer)
	{
		assert(isFrameStarted && "Can't begin render pass if frame is not started");
		assert(commandBuffer && "Can't begin render pass using a command buffer from an other frame");

		// Recording render pass cmd
		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = swapChain->GetRenderPass();
		renderPassInfo.framebuffer = swapChain->GetFrameBuffer(currentImageIndex);
		renderPassInfo.renderArea.offset = { 0,0 };
		renderPassInfo.renderArea.extent = swapChain->GetSwapChainExtent();

		std::array<VkClearValue, 2> clearValues{};
		clearValues[0].color = { 0.01f, 0.01f, 0.01f, 1.0f };
		clearValues[1].depthStencil = { 1, 0 };

		renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassInfo.pClearValues = clearValues.data();

		vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = static_cast<float>(swapChain->GetSwapChainExtent().width);
		viewport.height = static_cast<float>(swapChain->GetSwapChainExtent().height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		VkRect2D scissor{ {0,0}, swapChain->GetSwapChainExtent() };
		vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
		vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

	}

	void Renderer::EndSwapChainRenderPass(VkCommandBuffer commandBuffer)
	{
		assert(isFrameStarted && "Can't end render pass if frame is not started");
		assert(commandBuffer && "Can't end render pass using a command buffer from an other frame");

		// Ending recording
		vkCmdEndRenderPass(commandBuffer);
	}

}