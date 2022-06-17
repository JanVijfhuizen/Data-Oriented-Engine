#pragma once

namespace game
{
	class GameState;
	class SystemManager;

	struct SystemInfo final
	{
		GameState const* gameState = nullptr;
		EngineOutData const* engineOutData = nullptr;
	};

	class System
	{
		friend SystemManager;

	protected:
		virtual void Allocate(const SystemInfo& info) = 0;
		virtual void Free(const SystemInfo& info) = 0;

		virtual void Awake(const SystemInfo& info);;
		virtual void Start(const SystemInfo& info);;
		virtual void Update(const SystemInfo& info);;
	};
}
