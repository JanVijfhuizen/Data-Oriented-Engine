#pragma once
#include "SubTexture.h"
#include "StringView.h"

namespace vke
{
	struct Texture;

	namespace texture
	{
		struct TextureAtlasPartition final
		{
			size_t width = 1;
			jlb::StringView path{};
		};

		constexpr VkFormat DEFAULT_FORMAT = VK_FORMAT_R8G8B8A8_SRGB;
		constexpr VkImageLayout DEFAULT_LAYOUT = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

		[[nodiscard]] Texture LoadAsAtlas(const EngineData& info, 
			jlb::ArrayView<TextureAtlasPartition> partitions, jlb::ArrayView<SubTexture> outSubTextures, 
			glm::ivec2 nodeResolution, size_t atlasWidth);
		[[nodiscard]] Texture Load(const EngineData& info, jlb::StringView path);
		void Free(const EngineData& info, const Texture& texture);
	}
}
