#pragma once

namespace game
{
	class SystemManager;

	struct SystemInfo final
	{
		SystemManager* manager = nullptr;
		EngineOutData* engineOutData = nullptr;
	};

	class System
	{
		friend SystemManager;

	protected:
		virtual void Allocate(SystemInfo& info) = 0;
		virtual void Free(SystemInfo& info) = 0;

		virtual void Awake(SystemInfo& info) = 0;
		virtual void Start(SystemInfo& info) = 0;
		virtual void Update(SystemInfo& info) = 0;
	};
}
