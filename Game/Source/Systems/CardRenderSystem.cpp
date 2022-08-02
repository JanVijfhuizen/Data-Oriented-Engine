#include "pch.h"
#include "Systems/CardRenderSystem.h"

namespace game 
{
	jlb::StringView CardRenderSystem::GetTextureAtlasFilePath() const
	{
		return "Textures/atlas-card.png";
	}

	size_t CardRenderSystem::DefineCapacity(const vke::EngineData& info)
	{
		return 8;
	}
}
