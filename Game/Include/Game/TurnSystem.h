#pragma once
#include "VkEngine/Systems/GameSystem.h"

namespace game
{
	class TurnSystem final : public vke::GameSystem
	{
	public:
		float ticksPerSecond = 4;
		float timeMultiplier = 1;

		[[nodiscard]] bool GetIfTickEvent() const;
		[[nodiscard]] float GetTimeLerp() const;

	private:
		float _lastTickEventTime = 0;
		float _lerp = 0;
		bool _tickEventCalled = false;

		void Allocate(const vke::EngineData& info) override;
		void Free(const vke::EngineData& info) override;
		void Update(const vke::EngineData& info, jlb::Systems<vke::EngineData> systems) override;
	};
}
