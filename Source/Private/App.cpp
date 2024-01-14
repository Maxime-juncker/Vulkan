#include "../Public/App.h"
#include <stdexcept>
#include <iostream>
#include <vector>

namespace Application
{
	App::App()
	{
		// Creating the app
		CreatePipelineLayout();
		CreatePipeline();

		CreateCommandBuffers();
		CreateSyncObject();
	}

	App::~App()
	{
		// Cleating up memory
		vkDestroyPipelineLayout(device.GetDevice(), pipelineLayout, nullptr);

		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		{
			vkDestroySemaphore(device.GetDevice(), imageAvailableSemaphores[i], nullptr);
			vkDestroySemaphore(device.GetDevice(), renderFinishedSemaphores[i], nullptr);
			vkDestroyFence(device.GetDevice(), inFlightFences[i], nullptr);
		}

		swapChain.Cleanup();
		model.Cleanup();
		pipeline->Cleanup();


		// /!\		THE DEVICE NEED TO BE THE LAST CLEANUP IN LAST						/!\ //
		// /!\		lot of other things need the device event when jsut cleaning up		/!\ //
		device.Cleanup();
	}

	void App::Run()
	{
		// Run loop
		while (!window.ShouldClose())
		{
			glfwPollEvents();
			DrawFrame();
		}
		
		// Waiting for the gpu to finish his task before exiting
		vkDeviceWaitIdle(device.GetDevice());
	}

	void App::CreatePipelineLayout()
	{
		// Setting the pipeline
		VkPipelineLayoutCreateInfo pipelineCreateInfo{};
		pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineCreateInfo.setLayoutCount = 0;
		pipelineCreateInfo.pSetLayouts = nullptr;
		pipelineCreateInfo.pushConstantRangeCount = 0;
		pipelineCreateInfo.pPushConstantRanges = nullptr;
		if (vkCreatePipelineLayout(device.GetDevice(), &pipelineCreateInfo, nullptr, &pipelineLayout) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create pipeline layout");
		}
	}

	void App::CreatePipeline()
	{
		// Creating the pipeline config and layout
		auto pipelineConfig = Pipeline::DefaultPiplineConfigInfo(swapChain.GetWidth(), swapChain.GetHeight());
		pipelineConfig.renderPass = swapChain.GetRenderPass();
		pipelineConfig.pipelineLayout = pipelineLayout;

		// Creating the pipeline using the parameter from before
		pipeline = std::make_unique<Pipeline>(
		device,
			"Ressources/Shaders/SimpleShader.vert.spv",
			"Ressources/Shaders/SimpleShader.frag.spv",
			pipelineConfig);
	}

	void App::CreateCommandBuffers()
	{
		// Make sure that the command buffer is the same size than the Swap chain framebuffers
		commandBuffers.resize( swapChain.GetSwapChainFramebuffers().size());

		// Creating the alloc info for the buffers
		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.commandPool = device.GetCommandPool();
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandBufferCount = (uint32_t)commandBuffers.size();

		// Allocating the buffers
		if (vkAllocateCommandBuffers(device.GetDevice(), &allocInfo, commandBuffers.data()) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to alocate command buffer");
		}

		// Setting the buffers
		for (size_t i = 0; i < commandBuffers.size(); i++)
		{
			RecordCommandBuffers(commandBuffers[i], i);
		}
	}

	void App::RecordCommandBuffers(VkCommandBuffer commandBuffer, uint32_t imageIndex)
	{
		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = 0; // Optional
		beginInfo.pInheritanceInfo = nullptr;

		if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to begin recording command buffer");
		}

		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = swapChain.GetRenderPass();
		renderPassInfo.framebuffer = swapChain.GetFrameBuffer(imageIndex);
		renderPassInfo.renderArea.offset = { 0,0 };
		renderPassInfo.renderArea.extent = swapChain.GetSwapChainExtent();

		VkClearValue clearColor = { { {0.0f,0.0f,0.0f,1.0f} } };
		renderPassInfo.clearValueCount = 1;
		renderPassInfo.pClearValues = &clearColor;

		vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->GetGraphicPipeline());

		VkBuffer vertexBuffers[]{ model.GetVertexBuffer() };
		VkBuffer indexBuffer { model.GetIndexBuffer() };

		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
		vkCmdBindIndexBuffer(commandBuffer, indexBuffer, 0, VK_INDEX_TYPE_UINT16);
		
		vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(model.indices.size()), 1, 0, 0, 0);

		vkCmdEndRenderPass(commandBuffer);

		if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to register command buffer");
		}
	}

	void App::CreateSyncObject()
	{
		// Setting the syncronisation gpu <=> gpu and gpu <=> cpu

		// Make sure that the vector are the same size as the max frame in flight
		imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
		renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
		inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);
		inFlightImages.resize(swapChain.GetImageCount(), VK_NULL_HANDLE);

		VkSemaphoreCreateInfo semaphoreInfo{};
		semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		VkFenceCreateInfo fenceInfo{};
		fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;

		// Note: Don't forget the set the fence to already signaled on creaton,
		//		 othewise we will wait the signal in the start of draw but it will never go
		fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT; 

		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		{
			// Creating the semathore and fence
			if (vkCreateSemaphore(device.GetDevice(), &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]) != VK_SUCCESS ||
				vkCreateSemaphore(device.GetDevice(), &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]) != VK_SUCCESS ||
				vkCreateFence(device.GetDevice(), &fenceInfo, nullptr, &inFlightFences[i]) != VK_SUCCESS)
			{
				throw std::runtime_error("Failed to create sync object");
			}

		}

	}

	void App::DrawFrame()
	{
		// Make sure to wait for the old frame finish
		vkWaitForFences(device.GetDevice(), 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);
	
		// Setting the next frame
		uint32_t imageIndex;
		VkResult result = vkAcquireNextImageKHR(device.GetDevice(), swapChain.GetSwapChain(), UINT64_MAX,
			imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);

		if (result == VK_ERROR_OUT_OF_DATE_KHR) // Swap chain need to be recreated
		{
			swapChain.RecreateSwapChain();
			return;
		}
		else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
		{
			throw std::runtime_error("Failed to acquiere swap chain image !");
		}
		
		// Only reset the fence if we are submitting work.
		vkResetFences(device.GetDevice(), 1, &inFlightFences[currentFrame]);

		// Command buffer stuff
		vkResetCommandBuffer(commandBuffers[currentFrame], 0);
		RecordCommandBuffers(commandBuffers[currentFrame], imageIndex);
		
		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

		VkSemaphore waitSemaphore[] = { imageAvailableSemaphores[currentFrame]};
		VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = waitSemaphore;
		submitInfo.pWaitDstStageMask = waitStages;

		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffers[currentFrame];

		VkSemaphore signalSemaphores[] = { renderFinishedSemaphores[currentFrame]};
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = signalSemaphores;

		// Sending the frame in the queue
		if (vkQueueSubmit(device.GetGraphicsQueue(), 1, &submitInfo, inFlightFences[currentFrame]) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to send command buffer");
		}

		VkPresentInfoKHR presentInfo{};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = signalSemaphores;

		VkSwapchainKHR swapChains[] = { swapChain.GetSwapChain() };
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = swapChains;
		presentInfo.pImageIndices = &imageIndex;

		presentInfo.pResults = nullptr; // Optional

		// Displaying the frame
		result = vkQueuePresentKHR(device.GetPresentQueue(), &presentInfo);
		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || window.framebufferResized) 
		{
			// Reset the frame buffer resized.
			window.framebufferResized = false;
			// We need to recreate the swap chain
			swapChain.RecreateSwapChain();
		}
		else if (result != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to present swap chain image !");
		}

		// Don't overload the gpu
		vkQueueWaitIdle(device.GetGraphicsQueue()); // probl

		// Cycle between frames
		currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
	}
	
}