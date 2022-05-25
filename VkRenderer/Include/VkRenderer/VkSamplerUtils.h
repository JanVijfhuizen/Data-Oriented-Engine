#pragma once

namespace vk
{
	struct App;

	namespace sampler
	{
		[[nodiscard]] VkSamplerCreateInfo CreateDefaultInfo(App& app);
	}
}