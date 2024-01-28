#pragma once
#include "Pipline.h"
#include "Device.h"
#include "GameObject.h"

#include <memory>
#include <vector>

namespace Application
{
	class RenderSystem
	{
	public:
		RenderSystem(Device& device, VkRenderPass renderPass);
		~RenderSystem();

		RenderSystem(const RenderSystem&) = delete;
		RenderSystem& operator=(const RenderSystem&) = delete;

		void RenderGameObjects(VkCommandBuffer commandBuffer, std::vector<GameObject> &gameObjects);
	private:
		void CreatePipelineLayout();
		void CreatePipeline(VkRenderPass renderPass);


		Device& device;
		std::unique_ptr<Pipeline> pipeline;
		VkPipelineLayout pipelineLayout;
	};
}