#pragma once
#include "Archetypes/PlayerArchetype.h"
#include "Systems/RenderSystem.h"
#include "Systems/TextSystem.h"
#include "Graphics/RenderTask.h"
#include "Systems/AnimationSystem.h"
#include "Archetypes/CursorArchetype.h"
#include "Systems/MovementSystem.h"

namespace game
{
	struct GameState final
	{
		// Archetypes.
		PlayerArchetype playerArchetype{};
		CursorArchetype cursorArchetype{};

		// Game Systems.
		MovementSystem movementSystem{};

		// Graphic Systems.
		AnimationSystem animationSystem{};
		RenderSystem<RenderTask> renderSystem{};
		TextSystem uiSystem{};
	} gameState;
}
