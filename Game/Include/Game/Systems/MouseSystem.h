#pragma once
#include "VkEngine/Systems/GameSystem.h"

namespace game
{
	// Handles the mouse visualization and behaviour.
	class MouseSystem final : public vke::GameSystem
	{
	public:
		[[nodiscard]] size_t GetHoveredObject() const;
		[[nodiscard]] bool GetPressedThisTurn() const;

	private:
		size_t _hoveredObject = SIZE_MAX;
		bool _pressed = false;
		bool _pressedThisTurn = false;
		
		void PreUpdate(const vke::EngineData& info, jlb::Systems<vke::EngineData> systems) override;
		void PostUpdate(const vke::EngineData& info, jlb::Systems<vke::EngineData> systems) override;
		void OnMouseInput(const vke::EngineData& info, jlb::Systems<vke::EngineData> systems, int key, int action) override;
	};
}
