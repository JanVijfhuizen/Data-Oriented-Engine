#include "pch.h"
#include "Archetypes/PlayerArchetype.h"
#include "Systems/RenderSystem.h"

void game::PlayerArchetype::Update(RenderSystem& renderSystem)
{
	for (auto& entity : *this)
	{
		auto& renderer = jlb::Get<Renderer>(entity);
		auto& transform = jlb::Get<Transform>(entity);

		auto task = RenderSystem::CreateDefaultTask(renderer, transform);
		renderSystem.Add(task);
	}
}
