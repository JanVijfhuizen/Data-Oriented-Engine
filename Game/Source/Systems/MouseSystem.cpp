#include "pch.h"
#include "Systems/MouseSystem.h"
#include "VkEngine/Systems/UIRenderSystem.h"

namespace game
{
	void MouseSystem::Allocate(const vke::EngineData& info)
	{
	}

	void MouseSystem::Free(const vke::EngineData& info)
	{
	}

	void MouseSystem::Update(const vke::EngineData& info, const jlb::Systems<vke::EngineData> systems)
	{
		if (!info.mouseAvailable)
			return;

		const auto& mousePos = info.mousePos;
		const auto sys = systems.GetSystem<vke::UIRenderSystem>();

		vke::UIRenderTask task{};
		task.transform.position = mousePos;
		task.transform.scale = 0.1f;

		const auto result = sys->TryAdd(task);
		assert(result);
	}
}
