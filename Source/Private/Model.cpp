#include "../Public/Model.h"

#include <cassert>
namespace Application
{
	Model::Model(Device& device, const std::vector<Vertex>& verticies) :
		device{device}
	{
		CreateVertexBuffers(verticies);
	}

	Model::~Model()
	{
		vkDestroyBuffer(device.GetDevice(), vertexBuffer, nullptr);
		vkFreeMemory(device.GetDevice(), vertexBufferMemory, nullptr);
	}

	std::vector<VkVertexInputBindingDescription> Model::Vertex::GetBindingDescriptions()
	{
		std::vector<VkVertexInputBindingDescription> bindingDescriptions(1);
		bindingDescriptions[0].binding = 0;
		bindingDescriptions[0].stride = sizeof(Vertex);
		bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		return bindingDescriptions;
	}

	std::vector<VkVertexInputAttributeDescription> Model::Vertex::GetAttributeDescriptions()
	{
		std::vector<VkVertexInputAttributeDescription> attributeDescriptions(2);
		// Vertex attribute
		attributeDescriptions[0].location = 0;
		attributeDescriptions[0].binding = 0;
		attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
		attributeDescriptions[0].offset = offsetof(Vertex, position);
		// Color attribute
		attributeDescriptions[1].location = 1;
		attributeDescriptions[1].binding = 0;
		attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[1].offset = offsetof(Vertex, color);
		return attributeDescriptions;
	}

	void Model::CreateVertexBuffers(const std::vector<Vertex>& verticies)
	{
		vertexCount = static_cast<uint32_t>(verticies.size());
		assert(vertexCount >= 3 && "We need at least 3 verticies to render something !");

		VkDeviceSize bufferSize = sizeof(verticies[0]) * vertexCount;
		device.CreateBuffer
		(
			bufferSize,
			VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			vertexBuffer,
			vertexBufferMemory
		);

		// Copying data to vertex buffer
		void* data;
		vkMapMemory(device.GetDevice(), vertexBufferMemory, 0, bufferSize, 0, &data);
		memcpy(data, verticies.data(), static_cast<size_t>(bufferSize));
		vkUnmapMemory(device.GetDevice(), vertexBufferMemory);
	}

	void Model::Bind(VkCommandBuffer commandBuffer)
	{
		VkBuffer buffers[]{ vertexBuffer };
		VkDeviceSize offsets[] = {0};
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);

	}

	void Model::Draw(VkCommandBuffer commandBuffer)
	{
		vkCmdDraw(commandBuffer, vertexCount, 1, 0, 0);

	}

}