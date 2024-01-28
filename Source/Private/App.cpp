#include "../Public/App.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include <stdexcept>
#include <array>

namespace Application
{
	struct SimplePushConstantData
	{
		glm::mat2 transform{ 1.0f };
		glm::vec2 offset;
		alignas(16) glm::vec3 color;
	};

	App::App()
	{
		LoadModels();
		CreatePipelineLayout();
		RecreateSwapChain();
		CreateCommandBuffers();
	}

	App::~App()
	{
		vkDestroyPipelineLayout(device.GetDevice(), pipelineLayout, nullptr);
	}

	void App::Run()
	{
		while (!window.ShouldClose())
		{
			glfwPollEvents();
			DrawFrame();
		}

		// Blocking cpu until gpu finish it's work
		vkDeviceWaitIdle(device.GetDevice());
	}

	void App::LoadModels()
	{
		std::vector<Model::Vertex> vertices
		{
			{{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
			{{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
			{{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}
		};

		model = std::make_unique<Model>(device, vertices);
	}

	void App::CreatePipelineLayout()
	{
		VkPushConstantRange pushConstantRange{};
		pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
		pushConstantRange.offset = 0;
		pushConstantRange.size = sizeof(SimplePushConstantData);

		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = 0;
		pipelineLayoutInfo.pSetLayouts = nullptr;
		pipelineLayoutInfo.pushConstantRangeCount = 1;
		pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;
		if (vkCreatePipelineLayout(device.GetDevice(), 
			&pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create pipeline layout");
		}
	}

	void App::CreatePipeline()
	{
		assert(swapChain != nullptr && "Cannot create pipeline before swap chain");
		assert(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

		PipelineConfigInfo pipelineConfig{};
		Pipeline::DefaultPipelineConfigInfo(pipelineConfig);
		pipelineConfig.renderPass = swapChain->GetRenderPass();
		pipelineConfig.pipelineLayout = pipelineLayout;
		
		// Creating pipeline
		pipeline = std::make_unique<Pipeline>
		(
			device,
			"Resources/Shaders/SimpleShader.vert.spv",
			"Resources/Shaders/SimpleShader.frag.spv",
			pipelineConfig
		);
	}

	void App::CreateCommandBuffers()
	{
		// Creating command buffers
		commandBuffers.resize(swapChain->GetImageCount());

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

	void App::FreeCommandBuffers()
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

	void App::DrawFrame()
	{
		uint32_t imageIndex;
		auto result = swapChain->AcquireNextImage(&imageIndex);
		
		// Window probably resized
		if (result == VK_ERROR_OUT_OF_DATE_KHR)
		{
			RecreateSwapChain();
			return;
		}

		if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
		{
			throw std::runtime_error("Failed to acquire swap chain image");
		}

		RecordCommandBuffer(imageIndex);
		result = swapChain->SubmitCommandBuffers(&commandBuffers[imageIndex], &imageIndex);
		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR ||
			window.WasWindowResized())
		{
			window.ResetWindowResizedFlag();
			RecreateSwapChain();
			return;
		}

		if (result != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to present swap chain image");
		}
	}

	void App::RecreateSwapChain()
	{
		auto extend = window.GetExtend();
		// User minimized the window
		while (extend.width =+ 0 || extend.height == 0)
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
			swapChain = std::make_unique<SwapChain>(
				device, extend, std::move(swapChain));
			if (swapChain->GetImageCount() != commandBuffers.size())
			{
				FreeCommandBuffers();
				CreateCommandBuffers();
			}
		}

		CreatePipeline();
	}

	void App::RecordCommandBuffer(int imageIndex)
	{
		static int frame = 0;
		frame = (frame + 1) % 100;

		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		if (vkBeginCommandBuffer(commandBuffers[imageIndex], &beginInfo) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to start to record command buffer");
		}

		// Recording render pass cmd
		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = swapChain->GetRenderPass();
		renderPassInfo.framebuffer = swapChain->GetFrameBuffer(imageIndex);
		renderPassInfo.renderArea.offset = { 0,0 };
		renderPassInfo.renderArea.extent = swapChain->GetSwapChainExtent();

		std::array<VkClearValue, 2> clearValues{};
		clearValues[0].color = { 0.01f, 0.01f, 0.01f, 1.0f };
		clearValues[1].depthStencil = { 1, 0 };

		renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassInfo.pClearValues = clearValues.data();

		vkCmdBeginRenderPass(commandBuffers[imageIndex], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
		
		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = static_cast<float>(swapChain->GetSwapChainExtent().width);
		viewport.height = static_cast<float>(swapChain->GetSwapChainExtent().height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		VkRect2D scissor{ {0,0}, swapChain->GetSwapChainExtent() };
		vkCmdSetViewport(commandBuffers[imageIndex], 0, 1, &viewport);
		vkCmdSetScissor(commandBuffers[imageIndex], 0, 1, &scissor);

		// binding
		pipeline->Bind(commandBuffers[imageIndex]);
		model->Bind(commandBuffers[imageIndex]);

		for (int j = 0; j < 4; j++)
		{
			SimplePushConstantData push{};
			push.offset = {-0.5f + frame * 0.02f, -0.4f + j * 0.25f };
			push.color = { 0.0f, 0.0f, 0.2f + 0.2f * j };

			vkCmdPushConstants(commandBuffers[imageIndex], pipelineLayout,
				VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
				0, sizeof(SimplePushConstantData), &push);

			model->Draw(commandBuffers[imageIndex]);
		}

		// Ending recording
		vkCmdEndRenderPass(commandBuffers[imageIndex]);
		if (vkEndCommandBuffer(commandBuffers[imageIndex]) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to record buffer");
		}
	}
}