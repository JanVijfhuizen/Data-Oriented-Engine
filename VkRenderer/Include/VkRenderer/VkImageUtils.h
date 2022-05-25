#pragma once
#include "ArrayView.h"

namespace vk
{
	struct App;

	namespace image
	{
		[[nodiscard]] VkImageCreateInfo CreateDefaultInfo(glm::ivec2 resolution, VkImageUsageFlags usage);
		[[nodiscard]] VkImageViewCreateInfo CreateViewDefaultInfo(VkImage image, VkFormat format = VK_FORMAT_R8G8B8A8_SRGB);

		void CopyBufferToImage(VkBuffer buffer, VkImage image, VkCommandBuffer cmdBuffer, glm::ivec2 resolution);
		void TransitionLayout(VkImage image, VkCommandBuffer cmdBuffer, VkImageLayout oldLayout,
			VkImageLayout newLayout, VkImageAspectFlags aspectFlags);
		void GetLayoutMasks(VkImageLayout layout, VkAccessFlags& outAccessFlags, VkPipelineStageFlags& outPipelineStageFlags);
		[[nodiscard]] static VkFormat FindSupportedFormat(App& app, jlb::ArrayView<VkFormat> candidates, VkImageTiling tiling,
			VkFormatFeatureFlags features);
	}
}
