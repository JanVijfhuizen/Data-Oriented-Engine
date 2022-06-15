#pragma once
#include "HashMap.h"
#include "System.h"
#include "Vector.h"

namespace game
{
	struct SystemManagerCreateInfo final
	{
		jlb::StackAllocator* allocator = nullptr;
		size_t length = 0;
	};

	class SystemManager final
	{
	public:
		void Allocate(jlb::StackAllocator& allocator, const SystemManagerCreateInfo& info);
		void Free(jlb::StackAllocator& allocator);

		template <typename T>
		void CreateSystem(jlb::StackAllocator& allocator);

		template <typename T>
		[[nodiscard]] T& Get();

	private:
		jlb::HashMap<System*> _systems{};
		jlb::Vector<jlb::AllocationID> _allocations{};
	};

	template <typename T>
	void SystemManager::CreateSystem(jlb::StackAllocator& allocator)
	{
		assert(_allocations.GetCount() < _allocations.GetLength());

		auto allocation = allocator.New<T>();
		_systems.Insert(allocation.ptr);
		_allocations.Add(allocation.id);
	}

	template <typename T>
	T& SystemManager::Get()
	{
		return _systems[typeid(T)];
	}
}
