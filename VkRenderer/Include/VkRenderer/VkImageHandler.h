#pragma once
#include "ArrayView.h"

namespace vk
{
	struct App;

	class ImageHandler final
	{
	public:
		[[nodiscard]] static VkImageCreateInfo CreateDefaultInfo(glm::ivec2 resolution, VkImageUsageFlags usage);
		[[nodiscard]] static VkImageViewCreateInfo CreateViewDefaultInfo();

		static void CopyBufferToImage(VkBuffer buffer, VkImage image, VkCommandBuffer cmdBuffer, glm::ivec2 resolution);
		static void TransitionLayout(VkImage image, VkCommandBuffer cmdBuffer, VkImageLayout oldLayout,
			VkImageLayout newLayout, VkImageAspectFlags aspectFlags);
		static void GetLayoutMasks(VkImageLayout layout, VkAccessFlags& outAccessFlags, VkPipelineStageFlags& outPipelineStageFlags);
		[[nodiscard]] static VkFormat FindSupportedFormat(App& app, jlb::ArrayView<VkFormat> candidates, VkImageTiling tiling,
			VkFormatFeatureFlags features);
	};
}
