#include "pch.h"
#include "DemoGame.h"
#include "VkEngine/Systems/EntityRenderSystem.h"

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
		const auto sys = systems.GetSystem<vke::EntityRenderSystem>();
		size_t spacing = 50;

		for (size_t i = 0; i < 8192; ++i)
		{
			vke::EntityRenderTask task{};
			task.transform = {};
			task.transform.position.x = fmod(rand(), spacing) * 2 - spacing;
			task.transform.position.y = fmod(rand(), spacing) * 2 - spacing;
			const auto result = sys->TryAdd(task);
			assert(result);
		}
	}

	void DemoGame::Allocate(const vke::EngineData& info)
	{
	}

	void DemoGame::Free(const vke::EngineData& info)
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
