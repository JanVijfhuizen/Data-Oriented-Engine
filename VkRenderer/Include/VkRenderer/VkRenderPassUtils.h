#pragma once
#include "ArrayView.h"

namespace vk::renderPass
{
	[[nodiscard]] VkAttachmentDescription CreateAttachmentDescriptionDefaultInfo();
	[[nodiscard]] VkAttachmentReference CreateAttachmentReferenceDefaultInfo();
	[[nodiscard]] VkSubpassDescription CreateSubpassDescriptionDefaultInfo();
	[[nodiscard]] VkSubpassDependency CreateSubpassDependencyDefaultInfo();

	[[nodiscard]] VkRenderPassCreateInfo CreateDefaultInfo(
		jlb::ArrayView<VkAttachmentDescription> attachmentDescriptions,
		jlb::ArrayView<VkSubpassDescription> subpassDescriptions,
		jlb::ArrayView<VkSubpassDependency> subpassDependencies);

	[[nodiscard]] VkRenderPassBeginInfo CreateBeginDefaultInfo();
}
