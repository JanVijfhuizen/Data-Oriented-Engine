#pragma once
#include "System.h"

namespace game
{
	class ResourceManager final : public System
	{
	protected:
		void Allocate(SystemInfo& info) override;
		void Free(SystemInfo& info) override;
	};
}
