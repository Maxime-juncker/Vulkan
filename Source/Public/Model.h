#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "Device.h"

#include <glm/glm.hpp>
#include <vector>
#include <array>

namespace Application
{
	struct Vertex
	{
		glm::vec2 pos;
		glm::vec3 color;

		static VkVertexInputBindingDescription getBindingDescription()
		{
			VkVertexInputBindingDescription bindingDescription{};
			bindingDescription.binding + 0;
			// Note : We need to specifies the number of bytes from one entry to the next
			bindingDescription.stride = sizeof(Vertex);
			bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

			return bindingDescription;
		}

		static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions()
		{
			std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions{};

			// Position attribute
			attributeDescriptions[0].binding = 0;
			attributeDescriptions[0].location = 0;
			attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
			attributeDescriptions[0].offset = 0;

			// Color attribute
			attributeDescriptions[1].binding = 0;
			attributeDescriptions[1].location = 1;
			attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
			// Note : the color attribute is at an offset in the vertex struct because it's the 2nd parameter
			attributeDescriptions[1].offset = offsetof(Vertex, color); 



			return attributeDescriptions;
		}
	};

	class Model
	{
	public:
		Model(Device device);
		~Model();

		VkBuffer GetVertexBuffer() { return vertexBuffer; }

		const std::vector<Vertex> vertices =
		{
			{{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
			{{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
			{{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
		};

		void CreateVertexBuffers();
	private:

		Device device;
		VkBuffer vertexBuffer;
		VkDeviceMemory vertexBufferMemory;
	};

}
