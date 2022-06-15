#include "pch.h"
#include "SystemManager.h"

namespace game
{
	void SystemManager::Allocate(jlb::StackAllocator& allocator, const size_t length)
	{
		_allocations.Allocate(allocator, length);
		_map.Allocate(allocator, length);
		_vector.Allocate(allocator, length);
	}

	void SystemManager::Free(jlb::StackAllocator& allocator)
	{
		_vector.Free(allocator);
		_map.Free(allocator);

		for (int32_t i = _allocations.GetLength() - 1; i >= 0; --i)
			allocator.MFree(_allocations[i]);

		_allocations.Free(allocator);
	}

	void SystemManager::Awake(EngineOutData& outData)
	{
		auto info = GetSystemInfo(outData);
		for (auto& sys : _vector)
			sys->Awake(info);
	}

	void SystemManager::Start(EngineOutData& outData)
	{
		auto info = GetSystemInfo(outData);
		for (auto& sys : _vector)
			sys->Start(info);
	}

	void SystemManager::Update(EngineOutData& outData)
	{
		auto info = GetSystemInfo(outData);
		for (auto& sys : _vector)
			sys->Update(info);
	}

	SystemInfo SystemManager::GetSystemInfo(EngineOutData& outData)
	{
		SystemInfo info{};
		info.engineOutData = &outData;
		info.manager = this;
		return info;
	}
}
