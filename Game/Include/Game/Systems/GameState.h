#pragma once
#include "VkEngine/Systems/GameSystem.h"

namespace game
{
	class GameState final : public vke::GameSystem
	{
		void Allocate(const vke::EngineData& info) override;
		void Free(const vke::EngineData& info) override;

		void Awake(const vke::EngineData& info, jlb::Systems<vke::EngineData> systems) override;
		void Exit(const vke::EngineData& info, jlb::Systems<vke::EngineData> systems) override;
	};
}
