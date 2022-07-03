#include "pch.h"
#include "Systems/GameState.h"
#include "VkEngine/Systems/SceneSystem.h"
#include "Scenes/DemoScene.h"

namespace game
{
	void GameState::Allocate(const vke::EngineData& info)
	{
	}

	void GameState::Free(const vke::EngineData& info)
	{
	}

	void GameState::Awake(const vke::EngineData& info, const jlb::Systems<vke::EngineData> systems)
	{
		systems.GetSystem<vke::SceneSystem>()->AddScene<demo::DemoScene>(info, systems);
	}

	void GameState::Exit(const vke::EngineData& info, const jlb::Systems<vke::EngineData> systems)
	{
		systems.GetSystem<vke::SceneSystem>()->PopScene(info, systems);
	}
}
