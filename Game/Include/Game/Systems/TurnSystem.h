#pragma once
#include "StackArray.h"
#include "VkEngine/Systems/GameSystem.h"

namespace game
{
	/*
	Handles the turn management and visualization in the game.
	You can play/pause, speed up/slow down and forward turns in this system.
	*/
	class TurnSystem final : public vke::GameSystem
	{
	public:
		// Holds the visual settings for the turn system.
		struct Visuals final
		{
			// Padding between the keys.
			int32_t padding = -2;
			// Height of the turn ui in screen space.
			float screenYCoordinates = .8f;
			// UI buttons scale this much if they have been pressed.
			float onPressedSizeMultiplier = 1.5f;
			// Duration of the movement/scaling animation, n milliseconds.
			float onPressedAnimDuration = 100;
			// Vertical movement for pressed animation, in screen space.
			float onPressedTimeVerticalOffsetMultiplier = .1f;
		} visuals{};

		// Gets if a new turn has been started this frame.
		[[nodiscard]] bool GetIfTickEvent() const;
		// The linear interpolation between 0 and 1 of the time between turns.
		[[nodiscard]] float GetTickLerp() const;
		[[nodiscard]] size_t GetTicksPerSecond() const;
		[[nodiscard]] bool GetIsPaused() const;

		void Pause();
		void PauseAtEndOfTick();
		void SkipToNextTick();

	private:
		jlb::StackArray<float, 5> _keyVerticalLerps{1};

		bool _paused = false;
		size_t _ticksPerSecond = 1;
		size_t _previousTicksPerSecond = 1;
		const size_t _maxTicksPerSecond = 16;

		float _time = 0;
		bool _tickCalled = true;
		float _lerp = 0;
		bool _pauseAtEndOfTick = false;
		bool _forwardToNextTick = false;
		
		void PreUpdate(const vke::EngineData& info, jlb::Systems<vke::EngineData> systems) override;
		void OnKeyInput(const vke::EngineData& info, const jlb::Systems<vke::EngineData> systems, int key, int action) override;
		void PressKey(size_t index);
	};
}
