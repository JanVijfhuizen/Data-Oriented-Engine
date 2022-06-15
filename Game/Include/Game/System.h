#pragma once

namespace game
{
	class SystemManager;

	struct SystemInfo final
	{
		SystemManager* manager = nullptr;
		EngineOutData const* engineOutData = nullptr;
	};

	class System
	{
		friend SystemManager;

	protected:
		virtual void Allocate(SystemInfo& info) = 0;
		virtual void Free(SystemInfo& info) = 0;

		virtual void Awake(SystemInfo& info);;
		virtual void Start(SystemInfo& info);;
		virtual void Update(SystemInfo& info);;
	};
}
