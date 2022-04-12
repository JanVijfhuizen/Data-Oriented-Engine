#pragma once
#include "ArrayView.h"

namespace vk
{
	class RenderPassHandler final
	{
	public:
		[[nodiscard]] static VkAttachmentDescription CreateAttachmentDescriptionDefaultInfo();
		[[nodiscard]] static VkAttachmentReference CreateAttachmentReferenceDefaultInfo();
		[[nodiscard]] static VkSubpassDescription CreateSubpassDescriptionDefaultInfo();
		[[nodiscard]] static VkSubpassDependency CreateSubpassDependencyDefaultInfo();

		[[nodiscard]] static VkRenderPassCreateInfo CreateDefaultInfo(
			jlb::ArrayView<VkAttachmentDescription> attachmentDescriptions,
			jlb::ArrayView<VkSubpassDescription> subpassDescriptions,
			jlb::ArrayView<VkSubpassDependency> subpassDependencies);

		[[nodiscard]] static VkRenderPassBeginInfo CreateBeginDefaultInfo();
	};
}
