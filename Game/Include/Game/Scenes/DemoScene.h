#pragma once
#include "Archetypes/PlayerArchetype.h"
#include "VkEngine/Systems/SceneSystem.h"

namespace game::demo
{
	// Silly little scene I use to prototype and test.
	class DemoScene final : public vke::Scene
	{
		Player _player{};
		PlayerArchetype _playerArchetype{};
		
		void PreUpdate(const vke::EngineData& info, jlb::Systems<vke::EngineData> systems) override;
		void PostUpdate(const vke::EngineData& info, jlb::Systems<vke::EngineData> systems) override;
		void EndFrame(const vke::EngineData& info, jlb::Systems<vke::EngineData> systems) override;

		void OnKeyInput(const vke::EngineData& info, jlb::Systems<vke::EngineData> systems, int key,
			int action) override;
		void OnMouseInput(const vke::EngineData& info, jlb::Systems<vke::EngineData> systems, int key,
			int action) override;
	};
}
