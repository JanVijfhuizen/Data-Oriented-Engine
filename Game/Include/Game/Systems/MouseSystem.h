#pragma once
#include "VkEngine/Systems/GameSystem.h"

namespace game
{
	// Handles the mouse visualization and behaviour.
	class MouseSystem final : public vke::GameSystem
	{
		void PreUpdate(const vke::EngineData& info, jlb::Systems<vke::EngineData> systems) override;
	};
}
