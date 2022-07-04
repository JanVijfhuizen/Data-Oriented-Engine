#pragma once
#include "VkEngine/Systems/GameSystem.h"

namespace game
{
	class MouseSystem final : public vke::GameSystem
	{
		void Allocate(const vke::EngineData& info) override;
		void Free(const vke::EngineData& info) override;
		void Update(const vke::EngineData& info, jlb::Systems<vke::EngineData> systems) override;
	};
}
