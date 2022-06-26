#pragma once
#include "VkEngine/Systems/GameSystem.h"
#include "VkEngine/Systems/SceneSystem.h"
#include "VkEngine/Systems/TaskSystem.h"
#include "VkEngine/Systems/EntityRenderSystem.h"

namespace game
{
	class DemoGame final : public vke::GameSystem, public vke::ITaskSystemSubscriber<vke::EntityRenderTask>
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

		size_t DefineUsage(const vke::EntityRenderTask&) override;
	};
}
