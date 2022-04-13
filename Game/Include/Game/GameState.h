#pragma once
#include "Archetypes/PlayerArchetype.h"
#include "Systems/RenderSystem.h"

namespace game
{
	struct GameState final
	{
		// Archetypes.
		PlayerArchetype playerArchetype;

		// Systems.
		RenderSystem renderSystem;
	} gameState;
}
