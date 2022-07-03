#include "VkEngine/pch.h"
#include "VkEngine/Systems/UIRenderSystem.h"

namespace game
{
	jlb::StringView UIRenderSystem::GetTextureAtlasFilePath() const
	{
		return "Textures/Atlas.png";
	}

	jlb::StringView UIRenderSystem::GetFragmentShaderPath() const
	{
		return "Shaders/frag.spv";
	}

	jlb::StringView UIRenderSystem::GetVertexShaderPath() const
	{
		return "Shaders/vert.spv";
	}
}
