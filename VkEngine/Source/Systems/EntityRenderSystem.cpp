#include "VkEngine/pch.h"
#include "VkEngine/Systems/EntityRenderSystem.h"

namespace vke
{
	jlb::StringView EntityRenderSystem::GetTextureAtlasFilePath() const
	{
		return "Textures/atlas.png";
	}

	jlb::StringView EntityRenderSystem::GetFragmentShaderPath() const
	{
		return "Shaders/frag.spv";
	}

	jlb::StringView EntityRenderSystem::GetVertexShaderPath() const
	{
		return "Shaders/vert.spv";
	}

	size_t EntityRenderSystem::DefineCapacity(const EngineData& info)
	{
		return ENTITY_RENDER_SYSTEM_CAPACITY;
	}
}
