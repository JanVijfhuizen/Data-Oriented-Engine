#pragma once
#include "SwapChain.h"
#include "VkEngine/Systems/GameSystem.h"

namespace game
{
	// Handles the mouse visualization and behaviour.
	class MouseSystem final : public vke::GameSystem
	{
	public:
		[[nodiscard]] size_t GetHoveredObject() const;

	private:
		size_t _hoveredObject = SIZE_MAX;
		bool _pressed = false;
		
		void PreUpdate(const vke::EngineData& info, jlb::Systems<vke::EngineData> systems) override;
		void OnMouseInput(const vke::EngineData& info, jlb::Systems<vke::EngineData> systems, int key, int action) override;
	};
}
