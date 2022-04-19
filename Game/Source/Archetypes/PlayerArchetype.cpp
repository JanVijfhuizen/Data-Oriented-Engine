#include "pch.h"
#include "Archetypes/PlayerArchetype.h"
#include "Systems/RenderSystem.h"

void game::PlayerArchetype::DefineResourceUsage(RenderSystem& renderSystem)
{
	renderSystem.IncreaseRequestedLength(GetLength());
}

void game::PlayerArchetype::OnUpdate(Entity& entity, RenderSystem& renderSystem)
{
	auto& renderer = jlb::Get<Renderer>(entity);
	auto& transform = jlb::Get<Transform>(entity);

	auto task = RenderSystem::CreateDefaultTask(renderer, transform);
	renderSystem.Add(task);
}
