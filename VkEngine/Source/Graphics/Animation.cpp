#include "VkEngine/pch.h"
#include "VkEngine/Graphics/Animation.h"
#include "VkEngine/Graphics/SubTexture.h"
#include "VkEngine/Graphics/TextureUtils.h"

namespace vke
{
	SubTexture Animation::Evaluate(SubTexture subTexture, const float delta)
	{
		lerp += delta;
		while (lerp > 1)
			lerp -= 1;
		while (lerp < 0)
			lerp += 1;
		const float chunkSize = vke::texture::GetChunkSize(subTexture, width);
		const float index = floorf(lerp * static_cast<float>(width));
		subTexture.lTop.x += chunkSize * index;
		subTexture.rBot.x -= chunkSize * (static_cast<float>(width) - index - 1);
		return subTexture;
	}
}
