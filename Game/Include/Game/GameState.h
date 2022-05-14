#pragma once
#include "Archetypes/PlayerArchetype.h"
#include "Systems/RenderSystem.h"
#include "Systems/TextSystem.h"
#include "Graphics/RenderTask.h"
#include "Systems/AnimationSystem.h"
#include "Archetypes/CursorArchetype.h"
#include "Systems/MovementSystem.h"
#include "Systems/CollisionSystem.h"

namespace game
{
	struct GameState final
	{
		// Archetypes.
		PlayerArchetype playerArchetype{};
		CursorArchetype cursorArchetype{};

		// Game Systems.
		MovementSystem movementSystem{};
		CollisionSystem collisionSystem{};

		// Graphic Systems.
		AnimationSystem animationSystem{};
		RenderSystem<RenderTask> renderSystem{};
		TextSystem uiSystem{};
	} gameState;
}
