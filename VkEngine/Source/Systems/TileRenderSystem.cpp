#include "VkEngine/pch.h"
#include "VkEngine/Systems/TileRenderSystem.h"

namespace vke
{
	jlb::StringView TileRenderSystem::GetTextureAtlasFilePath() const
	{
		return "Textures/atlas-tile.png";
	}

	jlb::StringView TileRenderSystem::GetFragmentShaderPath() const
	{
		return "Shaders/frag-tile.spv";
	}

	jlb::StringView TileRenderSystem::GetVertexShaderPath() const
	{
		return "Shaders/vert-tile.spv";
	}
}
