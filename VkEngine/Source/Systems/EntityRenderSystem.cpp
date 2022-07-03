#include "VkEngine/pch.h"
#include "VkEngine/Systems/EntityRenderSystem.h"

namespace vke
{
	jlb::StringView EntityRenderSystem::GetTextureAtlasFilePath() const
	{
		return "Textures/Atlas.png";
	}

	jlb::StringView EntityRenderSystem::GetFragmentShaderPath() const
	{
		return "Shaders/frag.spv";
	}

	jlb::StringView EntityRenderSystem::GetVertexShaderPath() const
	{
		return "Shaders/vert.spv";
	}

	size_t EntityRenderSystem::DefineMinimalUsage(const EngineData& info)
	{
		return 1024;
	}
}
