#pragma once
#include "SystemChain.h"
#include "SystemManager.h"

namespace game
{
	inline struct GameState final
	{
		SystemChain chain{};
		SystemManager systems{};
	} gameState;
}
