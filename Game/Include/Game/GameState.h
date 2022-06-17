#pragma once
#include "SystemChain.h"
#include "SystemManager.h"
#include "Systems/ResourceManager.h"
#include "SystemInfo.h"

namespace game
{
	inline class GameState final
	{
	public:
		// Outdated.
		SystemChain chain{};

		ResourceManager resourceManager{};
		SystemManager<SystemInfo> systems{};
	} gameState;
}
