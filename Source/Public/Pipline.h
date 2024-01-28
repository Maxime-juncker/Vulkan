#pragma once

#include "Device.h"
#include <string>
#include <vector>

namespace Application
{
	struct PipelineConfigInfo
	{
		//PipelineConfigInfo(const PipelineConfigInfo&) = delete;
		//PipelineConfigInfo& operator=(const PipelineConfigInfo&) = delete;

		VkPipelineViewportStateCreateInfo viewportInfo;
		VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
		VkPipelineRasterizationStateCreateInfo rasterizationInfo;
		VkPipelineMultisampleStateCreateInfo multisampleInfo;
		VkPipelineColorBlendAttachmentState colorBlendAttachment;
		VkPipelineColorBlendStateCreateInfo colorBlendInfo;
		VkPipelineDepthStencilStateCreateInfo depthStencilInfo;
		std::vector<VkDynamicState> dynamicStateEnables;
		VkPipelineDynamicStateCreateInfo dynamicStateInfo;
		VkPipelineLayout pipelineLayout = nullptr;
		VkRenderPass renderPass = nullptr;
		uint32_t subpass = 0;
	};

	class Pipeline
	{
	public:
		Pipeline(
			Device& device, 
			const std::string vertFilepath,
			const std::string fragFilepath,
			const PipelineConfigInfo configInfo);
		~Pipeline();

		Pipeline(const Pipeline&) = delete;
		Pipeline& operator=(const Pipeline&) = delete;

		static void DefaultPipelineConfigInfo(PipelineConfigInfo& configInfo);

		void Bind(VkCommandBuffer commandBuffer);
	private:
		static std::vector<char> ReadFile(const std::string& filepath);

		void CreatePipeline(
			const std::string vertFilepath,
			const std::string fragFilepath,
			const PipelineConfigInfo configInfo);

		void CreateShaderModule(const std::vector<char>& code, VkShaderModule* shaderModule);

		Device& device;
		VkPipeline graphicsPipeline;
		VkShaderModule vertShaderModule;
		VkShaderModule fragShaderModule;
	};
}