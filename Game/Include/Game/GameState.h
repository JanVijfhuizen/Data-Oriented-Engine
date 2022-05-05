#pragma once
#include "Archetypes/PlayerArchetype.h"
#include "Systems/RenderSystem.h"
#include "Systems/UISystem.h"
#include "Graphics/RenderTask.h"
#include "Systems/AnimationSystem.h"

namespace game
{
	struct GameState final
	{
		// Archetypes.
		PlayerArchetype playerArchetype{};

		// Systems.
		AnimationSystem animationSystem{};
		RenderSystem<RenderTask> renderSystem{};
		UISystem uiSystem{};
	} gameState;
}
