#pragma once

#include "Device.h"
#include <string>
#include <vector>

namespace Application
{
	struct PiplineConfigInfo
	{
		VkViewport viewport;
		VkRect2D scissor;
		VkPipelineViewportStateCreateInfo viewportInfo;
		VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
		VkPipelineRasterizationStateCreateInfo rasterizationInfo;
		VkPipelineMultisampleStateCreateInfo multisampleInfo;
		VkPipelineColorBlendAttachmentState colorBlendAttachement;
		VkPipelineColorBlendStateCreateInfo colorBlendInfo;
		VkPipelineDepthStencilStateCreateInfo depthStencilInfo;
		VkPipelineLayout pipelineLayout = nullptr;
		VkRenderPass renderPass = nullptr;
		uint32_t subpass = 0;
	};

	class Pipeline
	{
	public:
		Pipeline(
			Device& device, 
			const std::string& vertFilePath, 
			const std::string& fragFilePath,
			const PiplineConfigInfo& configInfo);

		~Pipeline();

		Pipeline(const Pipeline&) = delete;
		void operator=(const Pipeline&) = delete;

		static PiplineConfigInfo DefaultPiplineConfigInfo(uint32_t width, uint32_t height);

	private:
		static std::vector<char> ReadFile(const std::string& path);

		void CreateGraphicPipline(
			const std::string& vertFilePath,
			const std::string& fragFilePath,
			const PiplineConfigInfo& configInfo);

		void CreateShaderModule(const std::vector<char>& code, VkShaderModule* shaderModule);

		Device& device;
		VkPipeline graphicPipline;
		VkShaderModule vertShaderModule;
		VkShaderModule fragShaderModule;
	};
}