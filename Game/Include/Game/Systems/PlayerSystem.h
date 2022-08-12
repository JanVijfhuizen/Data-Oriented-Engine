#pragma once
#include "VkEngine/Systems/GameSystem.h"

namespace game
{
	// Since a lot of the player behaviour is based on mouse input, there needs to be a way to communicate intentions to the player.
	// This is that way, albeit not optimal.
	class PlayerSystem final : public vke::GameSystem
	{
	public:

	};
}
