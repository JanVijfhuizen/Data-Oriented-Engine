#pragma once
#include "VkEngine/Systems/GameSystem.h"

namespace game
{
	class GameManager final : public vke::GameSystem
	{
		void Awake(const vke::EngineData& info, jlb::Systems<vke::EngineData> systems) override;
		void Exit(const vke::EngineData& info, jlb::Systems<vke::EngineData> systems) override;
	};
}
