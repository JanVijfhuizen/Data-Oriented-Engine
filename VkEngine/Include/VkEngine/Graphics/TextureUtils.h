#pragma once

namespace jlb 
{
	class StringView;
}

namespace vke
{
	struct Texture;

	namespace texture
	{
		constexpr VkFormat DEFAULT_FORMAT = VK_FORMAT_R8G8B8A8_SRGB;
		constexpr VkImageLayout DEFAULT_LAYOUT = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

		[[nodiscard]] Texture Load(const EngineData& info, jlb::StringView path);
		void Free(const EngineData& info, const Texture& texture);
	}
}
