#pragma once

#define GLFW_INCLUDE_VULKAN
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#include <GLFW/glfw3.h>



#include "Device.h"
#include "SwapChain.h"

#include <glm/glm.hpp>
#include <vector>
#include <array>

namespace Application
{
	struct UniformBufferObject
	{
		alignas(16) glm::mat4 model;
		alignas(16) glm::mat4 view;
		alignas(16) glm::mat4 proj;
	};

	struct Vertex
	{
		glm::vec2 pos;
		glm::vec3 color;
		glm::vec2 texCoord;

		static VkVertexInputBindingDescription getBindingDescription()
		{
			VkVertexInputBindingDescription bindingDescription{};
			bindingDescription.binding = 0;
			// Note : We need to specifies the number of bytes from one entry to the next
			bindingDescription.stride = sizeof(Vertex);
			bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

			return bindingDescription;
		}

		static std::array<VkVertexInputAttributeDescription, 3> getAttributeDescriptions()
		{
			std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions{};

			// Position attribute
			attributeDescriptions[0].binding = 0;
			attributeDescriptions[0].location = 0;
			attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
			attributeDescriptions[0].offset = offsetof(Vertex, pos);

			// Color attribute
			attributeDescriptions[1].binding = 0;
			attributeDescriptions[1].location = 1;
			attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
			// Note : the color attribute is at an offset in the vertex struct because it's the 2nd parameter
			attributeDescriptions[1].offset = offsetof(Vertex, color); 

			// Coordinates
			attributeDescriptions[2].binding = 0;
			attributeDescriptions[2].location = 2;
			attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
			attributeDescriptions[2].offset = offsetof(Vertex, texCoord);

			return attributeDescriptions;
		}
	};

	class Model
	{
	public:
		Model(Device device, SwapChain swapChain);
		void Cleanup();

		// Getter
		VkBuffer GetVertexBuffer() { return vertexBuffer; }
		VkBuffer GetIndexBuffer() { return indexBuffer; }
		VkDescriptorSetLayout& GetDescriptorLayout() { return descriptorSetLayout; }
		VkDescriptorSet& GetDescriptorSets(int32_t index) { return descriptorSets[index]; }

		const std::vector<Vertex> vertices =
		{
			{{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f},  {1.0f, 0.0f}},
			{{0.5f, -0.5f},  {0.0f, 1.0f, 0.0f},  {0.0f, 0.0f}},
			{{0.5f, 0.5f},   {0.0f, 0.0f, 1.0f},  {0.0f, 1.0f}},
			{{-0.5f, 0.5f},  {1.0f, 1.0f, 1.0f},  {1.0f, 1.0f}}
		};

		// If we use more than 65535 unique vertices, we need to up the uint16_t to uint32_t.
		const std::vector<uint16_t> indices =
		{
			0,1,2,2,3,0
		};

		void UpdateUniformBuffer(uint32_t currentImage);
		void CopyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);
	private:
		// Buffer methodes
		void CreateVertexBuffers();
		void CreateIndexBuffers();
		void CreateUniformBuffers();
		// Sets methodes
		void CreateDescriptionSetLayout();
		void CreateDescriptorPool();
		void CreateDescriptorSet();

		// Image methodes
		void CreateTextureImage();
		void CreateImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling,
			VkImageUsageFlags usage, VkMemoryPropertyFlags properties, 
			VkImage& image, VkDeviceMemory& imageMemory);
		void TransitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
		void CreateTextureImageView();
		void CreateTextureSampler();

		// References
		Device device;
		SwapChain swapChain;

		// Buffers.
		VkBuffer vertexBuffer;
		VkDeviceMemory vertexBufferMemory;
		VkBuffer indexBuffer;
		VkDeviceMemory indexBufferMemory;
		// The uniform buffer will be calculate every frame
		// but we don't want the next frame to read the current uniform buffer
		// so each frame in flight will have it's on uniform buffer.
		std::vector<VkBuffer> uniformBuffers;
		std::vector<VkDeviceMemory> uniformBuffersMemory;
		std::vector<void*> uniformBuffersMapped;

		// Descriptors
		VkDescriptorSetLayout descriptorSetLayout;
		VkDescriptorPool descriptorPool;
		std::vector<VkDescriptorSet> descriptorSets;

		// Images
		VkImage textureImage;
		VkDeviceMemory textureImageMemory;
		VkImageView textureImageView;
		VkSampler textureSampler;
						
	};

}
