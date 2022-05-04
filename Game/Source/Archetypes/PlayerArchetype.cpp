#include "pch.h"
#include "Archetypes/PlayerArchetype.h"
#include "Systems/RenderSystem.h"

void game::PlayerArchetype::DefineResourceUsage(PlayerArchetypeInfo& info)
{
	info.renderSystem->IncreaseRequestedLength(GetLength());
}

void game::PlayerArchetype::OnUpdate(Player& entity, PlayerArchetypeInfo& info)
{
	entity.renderer.subTexture.rightBot = { 0.5, 1 };
	auto task = RenderSystem::CreateDefaultTask(entity.renderer, entity.transform);
	info.renderSystem->Add(task);
}
