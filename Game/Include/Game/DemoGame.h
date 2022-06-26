#pragma once
#include "VkEngine/Systems/GameSystem.h"
#include "VkEngine/Systems/SceneSystem.h"

namespace game
{
	class DemoGame final : public vke::GameSystem
	{
		class DemoScene final : public vke::Scene
		{
			void Allocate(const vke::EngineData& info, jlb::Systems<vke::EngineData> systems) override;
			void Free(const vke::EngineData& info, jlb::Systems<vke::EngineData> systems) override;
			void Update(const vke::EngineData& info, jlb::Systems<vke::EngineData> systems) override;
		};

		void Allocate(const vke::EngineData& info, jlb::Systems<vke::EngineData> systems) override;
		void Free(const vke::EngineData& info, jlb::Systems<vke::EngineData> systems) override;

		void Awake(const vke::EngineData& info, jlb::Systems<vke::EngineData> systems) override;
		void Exit(const vke::EngineData& info, jlb::Systems<vke::EngineData> systems) override;
	};
}
