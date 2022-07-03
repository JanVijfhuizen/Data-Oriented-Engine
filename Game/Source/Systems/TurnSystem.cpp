#include "pch.h"
#include "Systems/TurnSystem.h"
#include "JlbMath.h"

namespace game
{
	constexpr size_t _MAX_TICKS_PER_SECOND = 16;

	bool TurnSystem::GetIfTickEvent() const
	{
		return _tickCalled;
	}

	float TurnSystem::GetTimeLerp() const
	{
		return _lerp;
	}

	void TurnSystem::Allocate(const vke::EngineData& info)
	{
	}

	void TurnSystem::Free(const vke::EngineData& info)
	{
	}

	void TurnSystem::Update(const vke::EngineData& info, const jlb::Systems<vke::EngineData> systems)
	{
		if (_paused)
			return;

		const float dTicksPerSecond = 1.f / _previousTicksPerSecond;

		_time += info.deltaTime * 0.01f;
		_tickCalled = false;

		while (_timePreviousTick + dTicksPerSecond < _time)
		{
			_timePreviousTick += dTicksPerSecond;
			_tickCalled = true;
		}

		_lerp = fmod(_time, dTicksPerSecond) / dTicksPerSecond;
		_previousTicksPerSecond = _ticksPerSecond;
	}

	void TurnSystem::OnKeyInput(const vke::EngineData& info, const jlb::Systems<vke::EngineData> systems, 
		const int key, const int action)
	{
		vke::GameSystem::OnKeyInput(info, systems, key, action);

		// Adjust is paused.
		if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
			_paused = !_paused;

		// Adjust turn speed.
		if (key == GLFW_KEY_LEFT && action == GLFW_PRESS)
			_ticksPerSecond /= 2;
		if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS)
			_ticksPerSecond *= 2;
		_ticksPerSecond = jlb::math::Clamp<size_t>(_ticksPerSecond, 1, _MAX_TICKS_PER_SECOND);
	}
}
