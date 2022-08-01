#pragma once
#include "VkEngine/Systems/GameSystem.h"

namespace game
{
	// Handles the mouse visualization and behaviour.
	class MouseSystem final : public vke::GameSystem
	{
	public:
		enum class Key
		{
			left,
			right
		};

		[[nodiscard]] size_t GetHoveredObject() const;
		[[nodiscard]] bool GetIsPressed(Key key) const;
		[[nodiscard]] bool GetIsPressedThisTurn(Key key) const;
		[[nodiscard]] bool GetIsUIBlocking() const;

	private:
		struct KeyData final
		{
			bool pressed = false;
			bool pressedThisTurn = false;
		};

		size_t _hoveredObject = SIZE_MAX;
		KeyData _keys[2];
		bool _isUIBlocking = false;
		
		void PreUpdate(const vke::EngineData& info, jlb::Systems<vke::EngineData> systems) override;
		void PostUpdate(const vke::EngineData& info, jlb::Systems<vke::EngineData> systems) override;
		void OnMouseInput(const vke::EngineData& info, jlb::Systems<vke::EngineData> systems, int key, int action) override;
	};
}
