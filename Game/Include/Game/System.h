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
		virtual void Allocate(SystemInfo& info) = 0;
		virtual void Free(SystemInfo& info) = 0;
	};
}
