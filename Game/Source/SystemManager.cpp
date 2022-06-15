#include "pch.h"
#include "SystemManager.h"

namespace game
{
	void SystemManager::Allocate(jlb::StackAllocator& allocator, const SystemManagerCreateInfo& info)
	{
		_systems.Allocate(allocator, info.length);
		_allocations.Allocate(allocator, info.length);
	}

	void SystemManager::Free(jlb::StackAllocator& allocator)
	{
		_systems.Free(allocator);
		_allocations.Free(allocator);
	}
}
