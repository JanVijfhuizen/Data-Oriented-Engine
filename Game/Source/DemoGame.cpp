#include "pch.h"
#include "DemoGame.h"

namespace game
{
	void DemoGame::DemoScene::Allocate(const vke::EngineData& info, const jlb::Systems<vke::EngineData> systems)
	{
	}

	void DemoGame::DemoScene::Free(const vke::EngineData& info, const jlb::Systems<vke::EngineData> systems)
	{
	}

	void DemoGame::DemoScene::Update(const vke::EngineData& info, const jlb::Systems<vke::EngineData> systems)
	{
	}

	void DemoGame::Allocate(const vke::EngineData& info, const jlb::Systems<vke::EngineData> systems)
	{
	}

	void DemoGame::Free(const vke::EngineData& info, const jlb::Systems<vke::EngineData> systems)
	{
	}

	void DemoGame::Awake(const vke::EngineData& info, const jlb::Systems<vke::EngineData> systems)
	{
		systems.GetSystem<vke::SceneSystem>()->AddScene<DemoScene>(info, systems);
	}

	void DemoGame::Exit(const vke::EngineData& info, const jlb::Systems<vke::EngineData> systems)
	{
		systems.GetSystem<vke::SceneSystem>()->PopScene(info, systems);
	}
}
