#pragma once
#include "StackArray.h"
#include "VkEngine/Systems/GameSystem.h"

namespace game
{
	class TurnSystem final : public vke::GameSystem
	{
	public:
		struct Visuals final
		{
			// Padding between the keys.
			size_t padding = 4;
			float screenYCoordinates = .8f;
			float onPressedSizeMultiplier = 1.5f;
			// In milliseconds.
			float onPressedAnimDuration = 100;
			float onPressedTimeVerticalOffsetMultiplier = .1f;
		} visuals{};

		[[nodiscard]] bool GetIfTickEvent() const;
		[[nodiscard]] float GetTickLerp() const;

	private:
		jlb::StackArray<float, 5> _keyVerticalLerps{1};

		bool _paused = false;
		size_t _ticksPerSecond = 4;
		size_t _previousTicksPerSecond = 4;
		const size_t _maxTicksPerSecond = 16;

		float _time = 0;
		float _timePreviousTick = 0;
		bool _tickCalled = false;

		float _lerp = 0;

		void Allocate(const vke::EngineData& info) override;
		void Free(const vke::EngineData& info) override;
		void Update(const vke::EngineData& info, jlb::Systems<vke::EngineData> systems) override;
		void OnKeyInput(const vke::EngineData& info, const jlb::Systems<vke::EngineData> systems, int key, int action) override;
	};
}
