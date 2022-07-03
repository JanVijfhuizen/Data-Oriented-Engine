#include "pch.h"
#include "TurnSystem.h"

namespace game
{
	bool TurnSystem::GetIfTickEvent() const
	{
		return _tickEventCalled;
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
		const float time = info.time * 0.01f;
		const float dTicksPerSecond = 1.f / ticksPerSecond;
		const float dTimeMultiplier = 1.f / timeMultiplier;
		const float tickDuration = dTicksPerSecond * dTimeMultiplier;

		_tickEventCalled = false;
		while (time > _lastTickEventTime + tickDuration)
		{
			_lastTickEventTime += tickDuration;
			_tickEventCalled = true;
		}

		_lerp = fmod(time, tickDuration) / tickDuration;
	}
}
