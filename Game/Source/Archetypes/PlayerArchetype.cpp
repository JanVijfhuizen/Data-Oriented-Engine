#include "pch.h"
#include "Archetypes/PlayerArchetype.h"
#include "Systems/RenderSystem.h"

void game::PlayerArchetype::DefineResourceUsage(PlayerArchetypeInfo& info)
{
	info.renderSystem->IncreaseRequestedLength(GetLength());
}

void game::PlayerArchetype::OnUpdate(PlayerArchetypeInfo& info, 
	Character&, Collider&, Controller&, Renderer& renderer,
	ShadowCaster&, Transform& transform)
{
	auto task = RenderSystem::CreateDefaultTask(renderer, transform);
	info.renderSystem->Add(task);
}
