#pragma once
#include "System.h"
#include "Vector.h"
#include "Map.h"
#include <vcruntime_typeinfo.h>

namespace jlb
{
	/// <summary>
	/// Manages all systems that can interact with the engine.
	/// </summary>
	template <typename T>
	class SystemManager final
	{
	public:
		void Allocate(StackAllocator& allocator, size_t length = 0);
		void Free(StackAllocator& allocator, const T& data);

		void Awake(const T& data);
		void Start(const T& data);
		void Update(const T& data);
		void OnRecreateSwapChainAssets(const T& data);
		void OnKeyInput(const T& data, int key, int action);
		void OnMouseInput(const T& data, int key, int action);

		void Exit(const T& data);

		template <typename U>
		void CreateSystem(StackAllocator& allocator);

		template <typename U>
		[[nodiscard]] U* GetSystem();

	private:
		Map<System<T>*> _map{};
		Vector<System<T>*> _vector{};
		Vector<jlb::AllocationID> _allocations{};
	};

	template <typename T>
	void SystemManager<T>::Allocate(StackAllocator& allocator, size_t length)
	{
		_allocations.Allocate(allocator, length);
		_map.Allocate(allocator, length);
		_vector.Allocate(allocator, length);
	}

	template <typename T>
	void SystemManager<T>::Free(StackAllocator& allocator, const T& data)
	{
		for (int32_t i = _vector.GetCount() - 1; i >= 0; --i)
		{
			_vector[i]->Free(data, *this);
			allocator.MFree(_allocations[i]);
		}

		_vector.Free(allocator);
		_map.Free(allocator);
		_allocations.Free(allocator);
	}

	template <typename T>
	void SystemManager<T>::Awake(const T& data)
	{
		for (auto& sys : _vector)
			sys->Allocate(data, *this);

		for (auto& sys : _vector)
			sys->Awake(data, *this);
	}

	template <typename T>
	void SystemManager<T>::Start(const T& data)
	{
		for (auto& sys : _vector)
			sys->Start(data, *this);
	}

	template <typename T>
	void SystemManager<T>::Update(const T& data)
	{
		for (auto& sys : _vector)
			sys->Update(data, *this);
	}

	template <typename T>
	void SystemManager<T>::OnRecreateSwapChainAssets(const T& data)
	{
		for (auto& sys : _vector)
			sys->OnRecreateSwapChainAssets(data, *this);
	}

	template <typename T>
	void SystemManager<T>::OnKeyInput(const T& data, const int key, const int action)
	{
		for (auto& sys : _vector)
			sys->OnKeyInput(data, *this, key, action);
	}

	template <typename T>
	void SystemManager<T>::OnMouseInput(const T& data, const int key, const int action)
	{
		for (auto& sys : _vector)
			sys->OnMouseInput(data, *this, key, action);
	}

	template <typename T>
	void SystemManager<T>::Exit(const T& data)
	{
		for (auto& sys : _vector)
			sys->Exit(data, *this);
	}

	template <typename T>
	template <typename U>
	void SystemManager<T>::CreateSystem(StackAllocator& allocator)
	{
		assert(_allocations.GetCount() < _allocations.GetLength());

		auto allocation = allocator.New<T>();
		_map.Insert(allocation.ptr, typeid(U).hash_code());
		_vector.Add(allocation.ptr);
		_allocations.Add(allocation.id);
	}

	template <typename T>
	template <typename U>
	U* SystemManager<T>::GetSystem()
	{
		System<T>** ptr = _map.Contains(typeid(U).hash_code());
		return static_cast<U*>(*ptr);
	}
}
