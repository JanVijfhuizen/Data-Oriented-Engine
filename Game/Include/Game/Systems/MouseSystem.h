#pragma once
#include "SwapChain.h"
#include "VkEngine/Systems/GameSystem.h"

namespace game
{
	// Handles the mouse visualization and behaviour.
	class MouseSystem final : public vke::GameSystem
	{
	public:
		[[nodiscard]] size_t GetHoveredObject();

	private:
		jlb::SwapChain<size_t, 2> _hoveredObject{};
		bool _pressed = false;

		void Allocate(const vke::EngineData& info) override;
		void PreUpdate(const vke::EngineData& info, jlb::Systems<vke::EngineData> systems) override;
		void OnMouseInput(const vke::EngineData& info, jlb::Systems<vke::EngineData> systems, int key, int action) override;
	};
}
