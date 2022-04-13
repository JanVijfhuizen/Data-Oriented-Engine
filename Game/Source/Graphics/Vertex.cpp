#include "pch.h"
#include "Graphics/Vertex.h"
#include "Components/Transform.h"

namespace game
{
	jlb::StackArray<VkVertexInputBindingDescription, 2> Vertex::GetBindingDescriptions()
	{
		jlb::StackArray<VkVertexInputBindingDescription, 2> bindingDescriptions{};

		auto& vertexData = bindingDescriptions[0];
		vertexData.binding = 0;
		vertexData.stride = sizeof(Vertex);
		vertexData.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		auto& instanceData = bindingDescriptions[1];
		instanceData.binding = 1;
		instanceData.stride = sizeof(glm::mat4);
		instanceData.inputRate = VK_VERTEX_INPUT_RATE_INSTANCE;
		return bindingDescriptions;
	}

	jlb::StackArray<VkVertexInputAttributeDescription, 6> Vertex::GetAttributeDescriptions()
	{
		jlb::StackArray<VkVertexInputAttributeDescription, 6> attributeDescriptions{};

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
		row1.location = 0;
		row1.format = VK_FORMAT_R32G32B32A32_SFLOAT;
		row1.offset = 0;

		auto& row2 = attributeDescriptions[3];
		row2.binding = 1;
		row2.location = 1;
		row2.format = VK_FORMAT_R32G32B32A32_SFLOAT;
		row2.offset = sizeof(glm::vec4);

		auto& row3 = attributeDescriptions[4];
		row3.binding = 1;
		row3.location = 2;
		row3.format = VK_FORMAT_R32G32B32A32_SFLOAT;
		row3.offset = sizeof(glm::vec4) * 2;

		auto& row4 = attributeDescriptions[5];
		row4.binding = 1;
		row4.location = 3;
		row4.format = VK_FORMAT_R32G32B32A32_SFLOAT;
		row4.offset = sizeof(glm::vec4) * 3;

		return attributeDescriptions;
	}
}