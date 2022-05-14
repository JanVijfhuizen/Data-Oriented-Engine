#pragma once
#include "Archetypes/PlayerArchetype.h"
#include "Systems/TextSystem.h"
#include "Archetypes/CursorArchetype.h"

namespace game
{
	struct GameState final
	{
		// Archetypes.
		PlayerArchetype playerArchetype{};
		CursorArchetype cursorArchetype{};

		SystemChain chain{};
	} gameState;
}
