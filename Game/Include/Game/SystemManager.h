#pragma once
#include "System.h"
#include "Vector.h"
#include "Map.h"
#include <vcruntime_typeinfo.h>

namespace game
{
	class SystemManager final
	{
	public:
		void Allocate(jlb::StackAllocator& allocator, size_t length = 0);
		void Free(const EngineOutData& outData);

		void Awake(const EngineOutData& outData);
		void Start(const EngineOutData& outData);
		void Update(const EngineOutData& outData);

		template <typename T>
		void CreateSystem(jlb::StackAllocator& allocator);

		template <typename T>
		[[nodiscard]] T& Get();

	private:
		jlb::Map<System*> _map{};
		jlb::Vector<System*> _vector{};
		jlb::Vector<jlb::AllocationID> _allocations{};

		[[nodiscard]] SystemInfo GetSystemInfo(const EngineOutData& outData);
	};

	template <typename T>
	void SystemManager::CreateSystem(jlb::StackAllocator& allocator)
	{
		assert(_allocations.GetCount() < _allocations.GetLength());

		auto allocation = allocator.New<T>();
		_map.Insert(allocation.ptr, typeid(T).hash_code());
		_vector.Add(allocation.ptr);
		_allocations.Add(allocation.id);
	}

	template <typename T>
	T& SystemManager::Get()
	{
		return *_map.Contains(typeid(T).hash_code());
	}
}
