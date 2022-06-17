#pragma once
#include "Texture.h"
#include "SubTexture.h"

namespace jlb 
{
	class StringView;
}

namespace game::texture
{
	[[nodiscard]] VkFormat GetFormat();
	[[nodiscard]] VkImageLayout GetImageLayout();
	[[nodiscard]] Texture Load(const EngineData& EngineData, jlb::StringView path);
	void Free(const EngineData& EngineData, Texture& texture);

	[[nodiscard]] SubTexture GenerateSubTexture(const Texture& texture, size_t chunkSize, glm::ivec2 lTop, glm::ivec2 rBot);
	[[nodiscard]] SubTexture GenerateSubTexture(const Texture& texture, size_t chunkSize, size_t index);
	[[nodiscard]] glm::ivec2 IndexToCoordinates(const Texture& texture, size_t chunkSize, size_t index);
}
