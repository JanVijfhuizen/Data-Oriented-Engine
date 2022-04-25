#include "pch.h"
#include "Graphics/Vertex.h"

namespace game
{
	jlb::StackArray<VkVertexInputBindingDescription, 1> Vertex::GetBindingDescriptions()
	{
		jlb::StackArray<VkVertexInputBindingDescription, 1> bindingDescriptions{};
		auto& vertexData = bindingDescriptions[0];
		vertexData.binding = 0;
		vertexData.stride = sizeof(Vertex);
		vertexData.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		return bindingDescriptions;
	}

	jlb::StackArray<VkVertexInputAttributeDescription, 2> Vertex::GetAttributeDescriptions()
	{
		jlb::StackArray<VkVertexInputAttributeDescription, 2> attributeDescriptions{};

		auto& position = attributeDescriptions[0];
		position.binding = 0;
		position.location = 0;
		position.format = VK_FORMAT_R32G32_SFLOAT;
		position.offset = offsetof(Vertex, position);

		auto& texCoords = attributeDescriptions[1];
		texCoords.binding = 0;
		texCoords.location = 1;
		texCoords.format = VK_FORMAT_R32G32_SFLOAT;
		texCoords.offset = offsetof(Vertex, textureCoordinates);

		return attributeDescriptions;
	}
}