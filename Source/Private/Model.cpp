#include "../Public/Model.h"
#include "../Public/Device.h"
#include "../Public/App.h"

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
// std includes.
#include <chrono>
#include <stdexcept>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"


namespace Application
{
	Model::Model(Device device, SwapChain swapChain) : device{device}, swapChain{swapChain}
	{
		CreateTextureImage();
		CreateVertexBuffers();
		CreateIndexBuffers();
		CreateUniformBuffers();
		CreateDescriptionSetLayout();
		CreateDescriptorPool();
		CreateDescriptorSet();
	}

	void Model::Cleanup()
	{

		vkDestroyBuffer(device.GetDevice(), vertexBuffer, nullptr);
		vkFreeMemory(device.GetDevice(), vertexBufferMemory, nullptr);

		vkDestroyBuffer(device.GetDevice(), indexBuffer, nullptr);
		vkFreeMemory(device.GetDevice(), indexBufferMemory, nullptr);

		for (size_t i = 0; i < App::MAX_FRAMES_IN_FLIGHT; i++)
		{
			vkDestroyBuffer(device.GetDevice(), uniformBuffers[i], nullptr);
			vkFreeMemory(device.GetDevice(), uniformBuffersMemory[i], nullptr);
		}

		vkDestroyDescriptorSetLayout(device.GetDevice(), descriptorSetLayout, nullptr);

		vkDestroyDescriptorPool(device.GetDevice(), descriptorPool, nullptr);
		vkDestroyDescriptorSetLayout(device.GetDevice(), descriptorSetLayout, nullptr);

		// Cleaning the texture
		vkDestroyImage(device.GetDevice(), textureImage, nullptr);
		vkFreeMemory(device.GetDevice(), textureImageMemory, nullptr);
	}

	void Model::CreateVertexBuffers()
	{
		VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

		// Creating a staging vertex buffer.
		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;
		device.CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, 
			stagingBuffer, stagingBufferMemory);
		
		// Setting the data inside the staging buffer.
		void* data;
		vkMapMemory(device.GetDevice(), stagingBufferMemory, 0, bufferSize, 0, &data);
		memcpy(data, vertices.data(), (size_t)bufferSize);
		vkUnmapMemory(device.GetDevice(), stagingBufferMemory);

		// Creating our vertex buffer.
		device.CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vertexBuffer, vertexBufferMemory);

		// Setting the vertex info.
		device.CopyBuffer(stagingBuffer, vertexBuffer, bufferSize);

		// Cleaning up the staging buffer.
		vkDestroyBuffer(device.GetDevice(), stagingBuffer, nullptr);
		vkFreeMemory(device.GetDevice(), stagingBufferMemory, nullptr);

	}

	void Model::CreateIndexBuffers()
	{
		VkDeviceSize bufferSize = sizeof(indices[0]) * indices.size();

		// Creating a staging index buffer.
		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;
		device.CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			stagingBuffer, stagingBufferMemory);

		// Setting the data inside the staging buffer.
		void* data;
		vkMapMemory(device.GetDevice(), stagingBufferMemory, 0, bufferSize, 0, &data);
		memcpy(data, indices.data(), (size_t)bufferSize);
		vkUnmapMemory(device.GetDevice(), stagingBufferMemory);

		// Creating our index buffer.
		device.CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, indexBuffer, indexBufferMemory);

		// Setting the index info.
		device.CopyBuffer(stagingBuffer, indexBuffer, bufferSize);

		// Cleaning up the staging buffer.
		vkDestroyBuffer(device.GetDevice(), stagingBuffer, nullptr);
		vkFreeMemory(device.GetDevice(), stagingBufferMemory, nullptr);
	}

	void Model::CreateUniformBuffers()
	{
		VkDeviceSize bufferSize = sizeof(UniformBufferObject);

		// Setting buffers size.
		uniformBuffers.resize(App::MAX_FRAMES_IN_FLIGHT);
		uniformBuffersMemory.resize(App::MAX_FRAMES_IN_FLIGHT);
		uniformBuffersMapped.resize(App::MAX_FRAMES_IN_FLIGHT);

		for (size_t i = 0; i < App::MAX_FRAMES_IN_FLIGHT; i++)
		{
			device.CreateBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
				uniformBuffers[i], uniformBuffersMemory[i]);

			vkMapMemory(device.GetDevice(), uniformBuffersMemory[i], 0, bufferSize, 0, &uniformBuffersMapped[i]);
		}
	}

	void Model::CreateDescriptionSetLayout()
	{
		// Creating uniform buffer.
		VkDescriptorSetLayoutBinding uboLayoutBinding{};
		uboLayoutBinding.binding = 0;
		uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		uboLayoutBinding.descriptorCount = 1;
		uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
		
		// Creating the layout info
		VkDescriptorSetLayoutCreateInfo layoutInfo{};
		layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutInfo.bindingCount = 1;
		layoutInfo.pBindings = &uboLayoutBinding;

		if (vkCreateDescriptorSetLayout(device.GetDevice(), &layoutInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create descriptor set layout !");
		}
	}

	void Model::CreateDescriptorPool()
	{
		// Creating descriptor pool.
		VkDescriptorPoolSize poolSize{};
		poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		poolSize.descriptorCount = static_cast<uint32_t>(App::MAX_FRAMES_IN_FLIGHT);
		
		VkDescriptorPoolCreateInfo poolInfo{};
		poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		poolInfo.poolSizeCount = 1;
		poolInfo.pPoolSizes = &poolSize;
		poolInfo.maxSets = static_cast<uint32_t>(App::MAX_FRAMES_IN_FLIGHT);

		if (vkCreateDescriptorPool(device.GetDevice(), &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create descriptor pool !");
		}
	}

	void Model::CreateDescriptorSet()
	{
		// Creating descriptor sets.
		std::vector<VkDescriptorSetLayout> layouts(App::MAX_FRAMES_IN_FLIGHT, descriptorSetLayout);
		VkDescriptorSetAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = descriptorPool;
		allocInfo.descriptorSetCount = static_cast<uint32_t>(App::MAX_FRAMES_IN_FLIGHT);
		allocInfo.pSetLayouts = layouts.data();

		descriptorSets.resize(App::MAX_FRAMES_IN_FLIGHT);
		if (vkAllocateDescriptorSets(device.GetDevice(), &allocInfo, descriptorSets.data()) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to allocate descriptor sets !");
		}

		// Populating descriptor sets
		for (size_t i = 0; i < App::MAX_FRAMES_IN_FLIGHT; i++)
		{
			// Creating buffer info
			VkDescriptorBufferInfo bufferInfo{};
			bufferInfo.buffer = uniformBuffers[i];
			bufferInfo.offset = 0;
			bufferInfo.range = sizeof(UniformBufferObject);

			// Creating VkWriteDescriptorSet 
			VkWriteDescriptorSet descriptorWrite{};
			descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrite.dstSet = descriptorSets[i];
			descriptorWrite.dstBinding = 0;
			descriptorWrite.dstArrayElement = 0;
			descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			descriptorWrite.descriptorCount = 1;
			descriptorWrite.pBufferInfo = &bufferInfo;

			vkUpdateDescriptorSets(device.GetDevice(), 1, &descriptorWrite, 0, nullptr);
		}
	}

	void Model::CreateTextureImage()
	{
		// Loading image.
		int texWidth, texHeight, texChannels;
		stbi_uc* pixels = stbi_load("Ressources/Textures/pizza.jpg", &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
		VkDeviceSize imageSize = texWidth * texHeight * 4; // 1 pixel = 4 byte (r,g,b,a).

		if (!pixels)
		{
			throw std::runtime_error("Failed to load texture image !");
		}

		VkBuffer stagingBuffer;
		VkDeviceMemory stagingMemory;

		// Creating a staging buffer (optimised for transfer).
		device.CreateBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			stagingBuffer, stagingMemory);

		// Copying the data from the pixel to the buffer.
		void* data;
		vkMapMemory(device.GetDevice(), stagingMemory, 0, imageSize, 0, &data);
		memcpy(data, pixels, static_cast<size_t>(imageSize));
		vkUnmapMemory(device.GetDevice(), stagingMemory);

		// Cleaning up the pixels array.
		stbi_image_free(pixels);

		// Creating the image.
		CreateImage(texWidth, texHeight, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL,
			VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			textureImage, textureImageMemory);

		// Converting the image to the right layout for data transfer.
		TransitionImageLayout(textureImage, VK_FORMAT_R8G8B8A8_SRGB,
			VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
		
		// Copying the info from the buffer to the image.
		CopyBufferToImage(stagingBuffer, textureImage, 
			static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight));

		// Converting the image to the right layout for shader use.
		TransitionImageLayout(textureImage, VK_FORMAT_R8G8B8A8_SRGB,
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

		vkDestroyBuffer(device.GetDevice(), stagingBuffer, nullptr);
		vkFreeMemory(device.GetDevice(), stagingMemory, nullptr);

	}

	void Model::CreateImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory)
	{
		// Setting the image infos.
		VkImageCreateInfo imageInfo{};
		imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageInfo.imageType = VK_IMAGE_TYPE_2D;
		imageInfo.extent.width = static_cast<uint32_t>(width);
		imageInfo.extent.height = static_cast<uint32_t>(height);
		imageInfo.extent.depth = 1;
		imageInfo.mipLevels = 1;
		imageInfo.arrayLayers = 1;
		imageInfo.format = VK_FORMAT_R8G8B8A8_SRGB; // Should be the same than the pixel buffer.
		imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL; // Eficient to access from the shader.
		imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		// We are going to set the image as the dst for the buffer and we will use it to sample.
		imageInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
		imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;

		// Creating the image.
		if (vkCreateImage(device.GetDevice(), &imageInfo, nullptr, &textureImage) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create image !");
		}

		// Handleling memory allocation.
		VkMemoryRequirements memRequirements;
		vkGetImageMemoryRequirements(device.GetDevice(), textureImage, &memRequirements);

		VkMemoryAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = device.FindMemoryType(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

		if (vkAllocateMemory(device.GetDevice(), &allocInfo, nullptr, &textureImageMemory) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to allocate image memory !");
		}

		vkBindImageMemory(device.GetDevice(), image, imageMemory, 0);
	}

	void Model::TransitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout)
	{
		// Creating buffer.
		VkCommandBuffer commandBuffer = device.BeginSingleTimeCommands();

		// Setting barrier info.
		VkImageMemoryBarrier barrier{};
		barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier.oldLayout = oldLayout;
		barrier.newLayout = newLayout;

		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

		barrier.image = image;
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		barrier.subresourceRange.levelCount = 1;
		barrier.subresourceRange.layerCount = 1;
		barrier.subresourceRange.baseMipLevel = 0;
		barrier.subresourceRange.baseArrayLayer = 0;

		VkPipelineStageFlags srcStage;
		VkPipelineStageFlags dstStage;

		if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
		{
			barrier.srcAccessMask = 0;
			barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

			srcStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			dstStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		}
		else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
		{
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

			srcStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
			dstStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		}
		else
		{
			throw std::invalid_argument("unsuported layout transition !");
		}

		vkCmdPipelineBarrier
		(
			commandBuffer,
			0 /* TODO */, 0, /* TODO */
			0,
			0, nullptr,
			0, nullptr,
			1, &barrier
		);

		// Ending the buffer.
		device.EndSingleTimeCommands(commandBuffer);
	}

	void Model::UpdateUniformBuffer(uint32_t currentImage)
	{
		static auto startTime = std::chrono::high_resolution_clock::now();

		auto currentTime = std::chrono::high_resolution_clock::now();
		float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

		UniformBufferObject ubo{};
		// Note : glm::rotate(glm::mat4(1.0f) = matrice identity 4
		ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(120.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		ubo.proj = glm::perspective(glm::radians(45.f), 
			swapChain.GetSwapChainExtent().width / (float)swapChain.GetSwapChainExtent().height, 0.1f, 10.0f);
		ubo.proj[1][1] *= -1; // GLM is design for opengl but the y axis is inverted in vulkan so we flip it.

		memcpy(uniformBuffersMapped[currentImage], &ubo, sizeof(ubo));

	}

	void Model::CopyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height)
	{
		VkCommandBuffer commandBuffer = device.BeginSingleTimeCommands();

		// Creating copy region.
		VkBufferImageCopy region{};
		region.bufferOffset = 0;
		region.bufferRowLength = 0;
		region.bufferImageHeight = 0;

		region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		region.imageSubresource.mipLevel = 0;
		region.imageSubresource.baseArrayLayer = 0;
		region.imageSubresource.layerCount = 1;

		region.imageOffset = { 0,0,0 };
		region.imageExtent =
		{
			width,
			height,
			1
		};

		vkCmdCopyBufferToImage
		(
			commandBuffer,
			buffer,
			image,
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			1,
			&region
		);

		device.EndSingleTimeCommands(commandBuffer);
	}
}