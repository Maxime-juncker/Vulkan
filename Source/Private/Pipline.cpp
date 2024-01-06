#include "../Public/Pipline.h"

#include <fstream>
#include <stdexcept>
#include <iostream>
#include <string>
#include <cassert>

namespace Application
{
	Pipeline::Pipeline(
		Device& device,
		const std::string& vertFilePath,
		const std::string& fragFilePath,
		const PiplineConfigInfo& configInfo) : device{ device }
	{
		CreateGraphicPipline(vertFilePath, fragFilePath, configInfo);
	}

	Pipeline::~Pipeline()
	{
		vkDestroyShaderModule(device.GetDevice(), vertShaderModule, nullptr);
		vkDestroyShaderModule(device.GetDevice(), fragShaderModule, nullptr);
		vkDestroyPipeline(device.GetDevice(), graphicPipline, nullptr);
	}

	std::vector<char> Pipeline::ReadFile(const std::string& path)
	{
		// Opening file
		// Note : std::ios::ate = seek end of file ( | std::ios::binary = read end as binary).	
		std::ifstream file{ path, std::ios::ate | std::ios::binary };

		if (!file.is_open())
		{
			// Could fail because of wrong file path or not having permission to open file.
			throw std::runtime_error("Failed to open file: " + path);
		}

		// Note : file.tellg() = get the current bit (we already are at the end of the file because of std::ios::ate)
		size_t fileSize = static_cast<size_t>(file.tellg());

		// Setting a buffer to the size of the file to hold our data.
		std::vector<char> buffer(fileSize);

		// Go to start of the file
		file.seekg(0);

		// Read the file and put the result in the buffer
		file.read(buffer.data(), fileSize);

		
		file.close();
		return buffer;
	}

	void Pipeline::CreateGraphicPipline(
		const std::string& vertFilePath,
		const std::string& fragFilePath,
		const PiplineConfigInfo& configInfo)
	{
		////

		//ENLEVER LE RETURN QUAND CE SERA BON

		////


		return;

		assert(configInfo.pipelineLayout != VK_NULL_HANDLE &&
			"Cannot create graphics pipeline: no pipelineLayout given in configInfo");
		assert(configInfo.renderPass != VK_NULL_HANDLE &&
			"Cannot create graphics pipeline: no renderPass given in configInfo");

		// Reading our shader from the compiled file (.spv)
		auto vertCode = ReadFile(vertFilePath);
		auto fragCode = ReadFile(fragFilePath);

		// Creating the shader module
		CreateShaderModule(vertCode, &vertShaderModule);
		CreateShaderModule(fragCode, &fragShaderModule);

		// Setting the frag and vert shader stage
		VkPipelineShaderStageCreateInfo shaderStage[2];
		shaderStage[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		shaderStage[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
		shaderStage[0].module = vertShaderModule;
		shaderStage[0].pName = "main";
		shaderStage[0].flags = 0;
		shaderStage[0].pName = nullptr;
		shaderStage[0].pSpecializationInfo = nullptr;

		shaderStage[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		shaderStage[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		shaderStage[1].module = fragShaderModule;
		shaderStage[1].pName = "main";
		shaderStage[1].flags = 0;
		shaderStage[1].pName = nullptr;
		shaderStage[1].pSpecializationInfo = nullptr;

		// Setting the vertex input info
		VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
		vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertexInputInfo.vertexAttributeDescriptionCount = 0;
		vertexInputInfo.vertexBindingDescriptionCount = 0;
		vertexInputInfo.pVertexAttributeDescriptions = nullptr;
		vertexInputInfo.pVertexBindingDescriptions = nullptr;


		// Setting the viewport info
		VkPipelineViewportStateCreateInfo viewportInfo{};
		viewportInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewportInfo.viewportCount = 1;
		viewportInfo.pViewports = &configInfo.viewport;
		viewportInfo.scissorCount = 1;
		viewportInfo.pScissors = &configInfo.scissor;

		// setting the pipeline infos
		VkGraphicsPipelineCreateInfo pipelineInfo{};
		pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipelineInfo.stageCount = 2;
		pipelineInfo.pStages = shaderStage;
		pipelineInfo.pVertexInputState = &vertexInputInfo;
		pipelineInfo.pInputAssemblyState = &configInfo.inputAssemblyInfo;
		pipelineInfo.pViewportState = &viewportInfo;
		pipelineInfo.pRasterizationState = &configInfo.rasterizationInfo;
		pipelineInfo.pColorBlendState = &configInfo.colorBlendInfo;
		pipelineInfo.pDepthStencilState = &configInfo.depthStencilInfo;
		pipelineInfo.pDynamicState = nullptr;
		pipelineInfo.pMultisampleState = &configInfo.multisampleInfo;

		pipelineInfo.layout = configInfo.pipelineLayout;
		pipelineInfo.renderPass = configInfo.renderPass;
		pipelineInfo.subpass = configInfo.subpass;

		pipelineInfo.basePipelineIndex = -1;
		pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

		// Creating the pipeline
		if (vkCreateGraphicsPipelines(device.GetDevice(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicPipline) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create graphics pipeline");
		}

	}

	PiplineConfigInfo Pipeline::DefaultPiplineConfigInfo(uint32_t width, uint32_t height)
	{
		PiplineConfigInfo configInfo{};

		configInfo.inputAssemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		configInfo.inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		configInfo.inputAssemblyInfo.primitiveRestartEnable = VK_FALSE;

		// Setting viewport
		configInfo.viewport.x = 0.0f;
		configInfo.viewport.y = 0.0f;
		configInfo.viewport.width = static_cast<float>(width);
		configInfo.viewport.height = static_cast<float>(height);
		configInfo.viewport.minDepth = 0.0f;
		configInfo.viewport.maxDepth = 1.0f;

		// Setting scissor (everything outside the scissor won't be displed)
		configInfo.scissor.offset = { 0,0 };
		configInfo.scissor.extent = { width, height };

		// Setting rasterizationInfo
		configInfo.rasterizationInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		configInfo.rasterizationInfo.depthClampEnable = VK_FALSE;
		configInfo.rasterizationInfo.rasterizerDiscardEnable = VK_FALSE;
		configInfo.rasterizationInfo.polygonMode = VK_POLYGON_MODE_FILL;
		configInfo.rasterizationInfo.lineWidth = 1.0f;
		configInfo.rasterizationInfo.cullMode = VK_CULL_MODE_NONE;
		configInfo.rasterizationInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
		configInfo.rasterizationInfo.depthBiasEnable = VK_FALSE;
		configInfo.rasterizationInfo.depthBiasConstantFactor = 0.0f; // Optional
		configInfo.rasterizationInfo.depthBiasClamp = 0.0f;			 // Optional
		configInfo.rasterizationInfo.depthBiasSlopeFactor = 0.0f;	 // Optional

		// setting multisampleInfo (to avoid aliasing)
		configInfo.multisampleInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		configInfo.multisampleInfo.sampleShadingEnable = VK_FALSE;
		configInfo.multisampleInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
		configInfo.multisampleInfo.minSampleShading = 1.0f;				// Optional
		configInfo.multisampleInfo.pSampleMask = nullptr;				// Optional
		configInfo.multisampleInfo.alphaToCoverageEnable = VK_FALSE;	// Optional
		configInfo.multisampleInfo.alphaToOneEnable = VK_FALSE;			// Optional

		// Setting Color blending attachement
		configInfo.colorBlendAttachement.colorWriteMask = VK_COLOR_COMPONENT_R_BIT
			| VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		configInfo.colorBlendAttachement.blendEnable = VK_FALSE;
		configInfo.colorBlendAttachement.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
		configInfo.colorBlendAttachement.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
		configInfo.colorBlendAttachement.colorBlendOp = VK_BLEND_OP_ADD;
		configInfo.colorBlendAttachement.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
		configInfo.colorBlendAttachement.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
		configInfo.colorBlendAttachement.alphaBlendOp = VK_BLEND_OP_ADD;
		
		// Setting Color blending Info
		configInfo.colorBlendInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		configInfo.colorBlendInfo.logicOpEnable = VK_FALSE;
		configInfo.colorBlendInfo.logicOp = VK_LOGIC_OP_COPY; // Optional
		configInfo.colorBlendInfo.attachmentCount = 1;
		configInfo.colorBlendInfo.pAttachments = &configInfo.colorBlendAttachement;
		configInfo.colorBlendInfo.blendConstants[0] = 0.0f; // Optional
		configInfo.colorBlendInfo.blendConstants[1] = 0.0f; // Optional
		configInfo.colorBlendInfo.blendConstants[2] = 0.0f; // Optional
		configInfo.colorBlendInfo.blendConstants[3] = 0.0f; // Optional

		// Setting Depth
		configInfo.depthStencilInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		configInfo.depthStencilInfo.depthTestEnable = VK_TRUE;
		configInfo.depthStencilInfo.depthWriteEnable = VK_TRUE;
		configInfo.depthStencilInfo.depthCompareOp = VK_COMPARE_OP_LESS;
		configInfo.depthStencilInfo.depthBoundsTestEnable = VK_FALSE;
		configInfo.depthStencilInfo.minDepthBounds = 0.0f;	// Optional
		configInfo.depthStencilInfo.maxDepthBounds = 1.0f;	// Optional
		configInfo.depthStencilInfo.stencilTestEnable = VK_FALSE;
		configInfo.depthStencilInfo.front = {};  // Optional
		configInfo.depthStencilInfo.back = {};   // Optional

		return configInfo;
	}

	void Pipeline::CreateShaderModule(const std::vector<char>& code, VkShaderModule* shaderModule)
	{
		VkShaderModuleCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = code.size();
		createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

		if (vkCreateShaderModule(device.GetDevice(), &createInfo, nullptr, shaderModule) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create shader module!");
		}
	}
}
