#include "pch.h"
#include "Systems/EntityRenderSystem.h"

namespace game
{
	RenderSystem<EntityRenderTask>::CreateInfo EntityRenderSystem::GetCreateInfo()
	{
		CreateInfo info{};
		info.atlasTexturePath = "Textures/atlas.png";
		info.vertPath = "Shaders/vert.spv";
		info.fragPath = "Shaders/frag.spv";
		return info;
	}
}
