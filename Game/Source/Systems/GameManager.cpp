#include "pch.h"
#include "Systems/GameManager.h"
#include "VkEngine/Systems/SceneSystem.h"
#include "Scenes/DemoScene.h"

namespace game
{
	void GameManager::Awake(const vke::EngineData& info, const jlb::Systems<vke::EngineData> systems)
	{
		systems.GetSystem<vke::SceneSystem>()->AddScene<demo::DemoScene>(info, systems);
	}

	void GameManager::Exit(const vke::EngineData& info, const jlb::Systems<vke::EngineData> systems)
	{
		systems.GetSystem<vke::SceneSystem>()->PopScene(info, systems);
	}
}
