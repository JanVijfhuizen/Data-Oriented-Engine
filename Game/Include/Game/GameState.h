#pragma once
#include "SystemChain.h"
#include "SystemManager.h"
#include "Systems/ResourceManager.h"

namespace game
{
	inline class GameState final
	{
	public:
		SystemChain chain{};

		ResourceManager resourceManager{};
		SystemManager systems{};
	} gameState;
}
