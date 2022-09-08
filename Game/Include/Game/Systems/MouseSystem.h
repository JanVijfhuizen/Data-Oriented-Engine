#pragma once
#include "Bounds.h"
#include "VkEngine/Systems/JobSystem.h"

namespace game
{
	typedef jlb::FBounds MouseInteractTask;

	// Handles the mouse visualization and behaviour.
	class MouseSystem final : public vke::JobSystem<MouseInteractTask>
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

		void OnPreUpdate(const vke::EngineData& info, jlb::Systems<vke::EngineData> systems, const jlb::NestedVector<jlb::TBounds<float>>& tasks) override;
		void OnPostUpdate(const vke::EngineData& info, jlb::Systems<vke::EngineData> systems, const jlb::NestedVector<jlb::TBounds<float>>& tasks) override;
		void OnMouseInput(const vke::EngineData& info, jlb::Systems<vke::EngineData> systems, int key, int action) override;
	};
}
