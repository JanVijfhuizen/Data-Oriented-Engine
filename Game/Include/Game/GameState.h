#pragma once
#include "Archetypes/PlayerArchetype.h"
#include "Systems/RenderSystem.h"
#include "Systems/UISystem.h"
#include "Graphics/RenderTask.h"

namespace game
{
	struct GameState final
	{
		// Archetypes.
		PlayerArchetype playerArchetype{};

		// Systems.
		RenderSystem<RenderTask> renderSystem{};
		UISystem uiSystem{};
	} gameState;
}
