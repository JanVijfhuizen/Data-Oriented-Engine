#pragma once

namespace vk
{
	class RenderPassHandler final
	{
	public:
		[[nodiscard]] static VkAttachmentDescription CreateAttachmentDescriptionDefaultInfo();
		[[nodiscard]] static VkAttachmentReference CreateAttachmentReferenceDefaultInfo();
		[[nodiscard]] static VkSubpassDependency CreateSubPassDependencyDefaultInfo();
		[[nodiscard]] static VkRenderPassCreateInfo CreateDefaultInfo();


	};
}