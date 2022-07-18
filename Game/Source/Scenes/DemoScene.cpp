#include "pch.h"
#include "Scenes/DemoScene.h"

#include <iostream>

#include "VkEngine/Systems/ThreadPoolSystem.h"

namespace game::demo
{
	void DemoScene::PreUpdate(const vke::EngineData& info, const jlb::Systems<vke::EngineData> systems)
	{
		Scene::PreUpdate(info, systems);
		_playerArchetype.PreUpdate(info, systems, _player);

		const auto sys = systems.GetSystem<vke::ThreadPoolSystem>();
		vke::ThreadPoolTask task{};
		task.func = [](const vke::EngineData& info, const jlb::Systems<vke::EngineData> systems, void* userPtr)
		{
			std::cout << "hello" << std::endl;
		};
		task.userPtr = this;

		jlb::NestableVector<vke::ThreadPoolTask> tasks{};
		tasks.Allocate(*info.dumpAllocator, 4, 2);
		for (int i = 0; i < 7; ++i)
		{
			tasks.Add(*info.dumpAllocator, task);
		}

		for (const auto& vector : tasks)
			task.func(info, systems, nullptr);

		/*
		for (int i = 0; i < 29; ++i)
		{
			const auto result = sys->TryAdd(info, task);
			assert(result != SIZE_MAX);
		}
		*/
	}

	void DemoScene::PostUpdate(const vke::EngineData& info, const jlb::Systems<vke::EngineData> systems)
	{
		Scene::PostUpdate(info, systems);
		_playerArchetype.PostUpdate(info, systems, _player);
	}

	void DemoScene::OnKeyInput(const vke::EngineData& info, const jlb::Systems<vke::EngineData> systems, const int key,
		const int action)
	{
		Scene::OnKeyInput(info, systems, key, action);
		_playerArchetype.OnKeyInput(info, systems, key, action);
	}

	void DemoScene::OnMouseInput(const vke::EngineData& info, const jlb::Systems<vke::EngineData> systems,
		const int key, const int action)
	{
		Scene::OnMouseInput(info, systems, key, action);
		_playerArchetype.OnMouseInput(info, systems, key, action);
	}
}
