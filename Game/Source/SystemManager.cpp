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

	void SystemManager::Free(const EngineOutData& outData)
	{
		auto& allocator = *outData.allocator;
		auto info = GetSystemInfo(outData);

		for (int32_t i = _vector.GetCount() - 1; i >= 0; --i)
		{
			_vector[i]->Free(info);
			allocator.MFree(_allocations[i]);
		}

		_vector.Free(allocator);
		_map.Free(allocator);
		_allocations.Free(allocator);
	}

	void SystemManager::Awake(const EngineOutData& outData)
	{
		auto info = GetSystemInfo(outData);
		for (auto& sys : _vector)
			sys->Awake(info);
	}

	void SystemManager::Start(const EngineOutData& outData)
	{
		auto info = GetSystemInfo(outData);
		for (auto& sys : _vector)
			sys->Start(info);
	}

	void SystemManager::Update(const EngineOutData& outData)
	{
		auto info = GetSystemInfo(outData);
		for (auto& sys : _vector)
			sys->Update(info);
	}

	SystemInfo SystemManager::GetSystemInfo(const EngineOutData& outData)
	{
		SystemInfo info{};
		info.engineOutData = &outData;
		info.manager = this;
		return info;
	}
}
