#pragma once
#include "VkEngine/Systems/SceneSystem.h"

namespace game
{
	class GameScene : public vke::Scene
	{
	public:
		void Allocate(const vke::EngineData& info, jlb::Systems<vke::EngineData> systems) override;
		void Free(const vke::EngineData& info, jlb::Systems<vke::EngineData> systems) override;
		void Update(const vke::EngineData& info, jlb::Systems<vke::EngineData> systems) override;
	};
}
