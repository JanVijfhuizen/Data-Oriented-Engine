﻿#pragma once
#include "System.h"
#include "Vector.h"
#include "Map.h"
#include <vcruntime_typeinfo.h>

namespace jlb
{
	template <typename T>
	class SystemManager;

	template <typename T>
	class Systems final
	{
		friend SystemManager<T>;
	public:
		template <typename U>
		void DefineSystem(StackAllocator& allocator, StackAllocator& tempAllocator, const T& data);

		template <typename U>
		[[nodiscard]] U* GetSystem();

	private:
		SystemManager<T>* _src = nullptr;
	};

	/// <summary>
	/// Manages all systems that can interact with the engine.
	/// </summary>
	template <typename T>
	class SystemManager final
	{
	public:
		[[nodiscard]] Systems<T> CreateProxy();

		template <typename U>
		void DefineSystem(StackAllocator& allocator, StackAllocator& tempAllocator, const T& data);

		void Allocate(StackAllocator& allocator, StackAllocator& tempAllocator);
		void Free(StackAllocator& allocator, const T& data);

		void Awake(const T& data);
		void Start(const T& data);
		void Update(const T& data);
		void OnRecreateSwapChainAssets(const T& data);
		void OnKeyInput(const T& data, int key, int action);
		void OnMouseInput(const T& data, int key, int action);

		void Exit(const T& data);

		template <typename U>
		[[nodiscard]] U* GetSystem();

	private:
		struct TempSystemData final
		{
			System<T>* ptr;
			AllocationID allocation;
			size_t hashCode;
		};

		Map<System<T>*> _map{};
		Vector<System<T>*> _vector{};
		Vector<AllocationID> _allocations{};
		Vector<TempSystemData> _tempSystemData{};
		bool _allocated = false;
	};

	template <typename T>
	void SystemManager<T>::Allocate(StackAllocator& allocator, StackAllocator& tempAllocator)
	{
		_allocated = true;
		const size_t length = _tempSystemData.GetLength();

		_allocations.Allocate(allocator, length);
		_map.Allocate(allocator, length);
		_vector.Allocate(allocator, length);

		for (auto& tempData : _tempSystemData)
		{
			_map.Insert(tempData.ptr, tempData.hashCode);
			_vector.Add(tempData.ptr);
			_allocations.Add(tempData.allocation);
		}

		_tempSystemData.Free(tempAllocator);
	}

	template <typename T>
	void SystemManager<T>::Free(StackAllocator& allocator, const T& data)
	{
		_allocated = false;
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
	void Systems<T>::DefineSystem(StackAllocator& allocator, StackAllocator& tempAllocator, const T& data)
	{
		return _src->DefineSystem(allocator, tempAllocator, data);
	}

	template <typename T>
	template <typename U>
	U* Systems<T>::GetSystem()
	{
		return _src->template GetSystem<U>();
	}

	template <typename T>
	Systems<T> SystemManager<T>::CreateProxy()
	{
		Systems<T> systems{};
		systems._src = this;
		return systems;
	}

	template <typename T>
	template <typename U>
	void SystemManager<T>::DefineSystem(StackAllocator& allocator, StackAllocator& tempAllocator, const T& data)
	{
		assert(!_allocated);
		assert(_allocations.GetCount() < _allocations.GetLength());

		auto allocation = allocator.New<U>();
		allocation.ptr.Allocate(data, *this);

		TempSystemData tempSystemData{};
		tempSystemData.allocation = allocation.id;
		tempSystemData.ptr = allocation.ptr;
		tempSystemData.hashCode = typeid(U).hash_code();
		_tempSystemData.Add(tempSystemData, tempAllocator, allocator);
	}

	template <typename T>
	template <typename U>
	U* SystemManager<T>::GetSystem()
	{
		System<T>** ptr = _map.Contains(typeid(U).hash_code());
		return static_cast<U*>(*ptr);
	}
}
