#pragma once
#include "VkEngine/Systems/UIRenderSystem.h"

namespace game
{
	class CardRenderSystem final : public vke::UIRenderSystem
	{
	protected:
		[[nodiscard]] jlb::StringView GetTextureAtlasFilePath() const override;
		[[nodiscard]] size_t DefineCapacity(const vke::EngineData& info) override;
	};
}
