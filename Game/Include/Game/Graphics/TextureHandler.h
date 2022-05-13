#pragma once
#include "Texture.h"
#include "SubTexture.h"

namespace jlb 
{
	class StringView;
}

namespace game
{
	class TextureHandler final
	{
	public:
		[[nodiscard]] static VkFormat GetTextureFormat();
		[[nodiscard]] static VkImageLayout GetImageLayout();
		[[nodiscard]] static Texture LoadTexture(const EngineOutData& engineOutData, jlb::StringView path);
		[[nodiscard]] static SubTexture GenerateSubTexture(const Texture& texture, size_t chunkSize, glm::ivec2 lTop, glm::ivec2 rBot);
		[[nodiscard]] static SubTexture GenerateSubTexture(const Texture& texture, size_t chunkSize, size_t index);
		[[nodiscard]] static glm::ivec2 IndexToCoordinates(const Texture& texture, size_t chunkSize, size_t index);
		[[nodiscard]] static SubTexture GenerateOffsettedSubTexture(const SubTexture& subTexture, glm::ivec2 lTop, glm::ivec2 rBot);
		static void FreeTexture(const EngineOutData& engineOutData, Texture& texture);
	};
}
