#pragma once
#include "StackArray.h"

namespace game
{
	struct Vertex final
	{
		typedef uint16_t Index;

		glm::vec2 position;
		glm::vec2 textureCoordinates;

		// Returns a vulkan description for the vertex binding.
		[[nodiscard]] static jlb::StackArray<VkVertexInputBindingDescription, 1> GetBindingDescriptions();
		// Returns a vulkan description for the vertex attributes.
		[[nodiscard]] static jlb::StackArray<VkVertexInputAttributeDescription, 2> GetAttributeDescriptions();
	};
}