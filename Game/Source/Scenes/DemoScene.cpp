#include "pch.h"
#include "Scenes/DemoScene.h"

#include <iostream>
#include "NestableVector.h"

namespace game::demo
{
	void DemoScene::PreUpdate(const vke::EngineData& info, const jlb::Systems<vke::EngineData> systems)
	{
		Scene::PreUpdate(info, systems);
		_playerArchetype.PreUpdate(info, systems, _player);

		jlb::NestableVector<float> nestable{};
		nestable.Allocate(*info.tempAllocator, 4);

		for (int i = 0; i < 9; ++i)
		{
			nestable.Add(*info.tempAllocator, i);
		}
		std::cout << std::endl;
		for (int i = 0; i < 9; ++i)
		{
			std::cout << nestable[i] << std::endl;
		}
		std::cout << std::endl;

		for (float& a : nestable)
		{
			std::cout << a << std::endl;
		}

		std::cout << std::endl;
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
