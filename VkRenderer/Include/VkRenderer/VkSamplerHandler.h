#pragma once

namespace vk
{
	struct App;

	class SamplerHandler final
	{
	public:
		[[nodiscard]] static VkSamplerCreateInfo CreateDefaultInfo(App& app);
	};
}