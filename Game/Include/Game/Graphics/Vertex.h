#pragma once
#include "StackArray.h"

namespace game
{
	struct Vertex final
	{
		typedef uint16_t Index;

		struct Instance final
		{
			glm::vec2 position;
			float rotation;
			float scale;
		};

		glm::vec2 position;
		glm::vec2 textureCoordinates;

		// Returns a vulkan description for the vertex binding.
		[[nodiscard]] static jlb::StackArray<VkVertexInputBindingDescription, 2> GetBindingDescriptions();
		// Returns a vulkan description for the vertex attributes.
		[[nodiscard]] static jlb::StackArray<VkVertexInputAttributeDescription, 5> GetAttributeDescriptions();
	};
}