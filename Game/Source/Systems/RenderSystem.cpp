#include "pch.h"
#include "Systems/RenderSystem.h"

namespace game
{
	void RenderSystem::Update(vk::App& app)
	{
		for (auto& task : *this)
		{
			// Do a render thing.
		}

		SetCount(0);
	}

	RenderTask RenderSystem::CreateDefaultTask(Renderer& renderer, Transform& transform)
	{
		RenderTask task{};
		return task;
	}
}
