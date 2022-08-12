﻿#pragma once
#include "Entities/Entity.h"
#include "VkEngine/Systems/GameSystem.h"

namespace game
{
	class PlayerArchetype;

	// Since a lot of the player behaviour is based on mouse input, there needs to be a way to communicate intentions to the player.
	// This is that way, albeit not optimal.
	class PlayerSystem final : public vke::GameSystem
	{
		friend PlayerArchetype;

	public:
		EntityId pickupEntity{};

	private:
		struct UpdateInfo final
		{
			glm::vec2 position{};
			bool keyArrowInput[4];
		} _updateInfo;
		
		void PreUpdate(const vke::EngineData& info, const jlb::Systems<vke::EngineData> systems) override;
	};
}