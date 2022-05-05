#include "pch.h"
#include "Archetypes/PlayerArchetype.h"
#include "Systems/RenderSystem.h"

void game::PlayerArchetype::DefineResourceUsage(PlayerArchetypeInfo& info)
{
	info.renderSystem->IncreaseRequestedLength(GetLength());
}

void game::PlayerArchetype::OnUpdate(Player& entity, PlayerArchetypeInfo& info)
{
	auto& transform = entity.transform;
	auto& renderer = entity.renderer;

	// Temp.
	renderer.subTexture.rightBot = { 0.5, 1 };

	RenderTask task{};
	auto& taskTransform = task.transform;
	taskTransform = transform;
	task.subTexture = renderer.subTexture;
	info.renderSystem->Add(task);
}
