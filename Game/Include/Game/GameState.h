#pragma once
#include "Archetypes/PlayerArchetype.h"
#include "Systems/RenderSystem.h"
#include "Systems/UISystem.h"

namespace game
{
	struct GameState final
	{
		// Archetypes.
		PlayerArchetype playerArchetype{};

		// Systems.
		RenderSystem renderSystem{};
		UISystem uiSystem{};
	} gameState;
}
