#pragma once
#include "Texture.h"

namespace jlb 
{
	class StringView;
}

namespace game
{
	class TextureHandler final
	{
	public:
		[[nodiscard]] static Texture LoadTexture(const EngineOutData& engineOutData, jlb::StringView path);
		static void FreeTexture(const EngineOutData& engineOutData, Texture& texture);
	};
}
