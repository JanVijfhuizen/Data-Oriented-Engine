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
		position.format = VK_FORMAT_R32G32_SFLOAT;
		position.offset = offsetof(Vertex, position);

		auto& texCoords = attributeDescriptions[1];
		texCoords.binding = 0;
		texCoords.location = 1;
		texCoords.format = VK_FORMAT_R32G32_SFLOAT;
		texCoords.offset = offsetof(Vertex, textureCoordinates);

		auto& instancePosition = attributeDescriptions[2];
		instancePosition.binding = 1;
		instancePosition.location = 2;
		instancePosition.format = VK_FORMAT_R32G32_SFLOAT;
		instancePosition.offset = offsetof(Instance, position);

		auto& instanceRotation = attributeDescriptions[3];
		instanceRotation.binding = 1;
		instanceRotation.location = 3;
		instanceRotation.format = VK_FORMAT_R32_SFLOAT;
		instanceRotation.offset = offsetof(Instance, rotation);

		auto& instanceScale = attributeDescriptions[4];
		instanceScale.binding = 1;
		instanceScale.location = 4;
		instanceScale.format = VK_FORMAT_R32_SFLOAT;
		instanceScale.offset = offsetof(Instance, scale);

		return attributeDescriptions;
	}
}