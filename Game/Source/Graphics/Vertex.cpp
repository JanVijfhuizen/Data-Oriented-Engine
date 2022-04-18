#include "pch.h"
#include "Graphics/Vertex.h"

namespace game
{
	jlb::StackArray<VkVertexInputBindingDescription, 2> Vertex::GetBindingDescriptions()
	{
		jlb::StackArray<VkVertexInputBindingDescription, 2> bindingDescriptions{};

		auto& vertexData = bindingDescriptions[0];
		vertexData.binding = 0;
		vertexData.stride = sizeof(Vertex);
		vertexData.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		auto& modelData = bindingDescriptions[1];
		modelData.binding = 1;
		modelData.stride = sizeof(Instance);
		modelData.inputRate = VK_VERTEX_INPUT_RATE_INSTANCE;
		return bindingDescriptions;
	}

	jlb::StackArray<VkVertexInputAttributeDescription, 5> Vertex::GetAttributeDescriptions()
	{
		jlb::StackArray<VkVertexInputAttributeDescription, 5> attributeDescriptions{};

		auto& position = attributeDescriptions[0];
		position.binding = 0;
		position.location = 0;
		position.format = VK_FORMAT_R32G32B32_SFLOAT;
		position.offset = offsetof(Vertex, position);

		auto& texCoords = attributeDescriptions[1];
		texCoords.binding = 0;
		texCoords.location = 1;
		texCoords.format = VK_FORMAT_R32G32_SFLOAT;
		texCoords.offset = offsetof(Vertex, textureCoordinates);

		auto& row1 = attributeDescriptions[2];
		row1.binding = 1;
		row1.location = 2;
		row1.format = VK_FORMAT_R32G32B32A32_SFLOAT;
		row1.offset = offsetof(Instance, position);

		auto& row2 = attributeDescriptions[3];
		row2.binding = 1;
		row2.location = 3;
		row2.format = VK_FORMAT_R32G32B32A32_SFLOAT;
		row2.offset = offsetof(Instance, rotation);

		auto& row3 = attributeDescriptions[4];
		row3.binding = 1;
		row3.location = 4;
		row3.format = VK_FORMAT_R32G32B32A32_SFLOAT;
		row3.offset = offsetof(Instance, scale);

		return attributeDescriptions;
	}
}