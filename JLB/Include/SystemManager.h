#pragma once
#include "System.h"
#include "Vector.h"
#include "Map.h"
#include <vcruntime_typeinfo.h>

namespace jlb
{
	template <typename T>
	class SystemManager;

	template <typename T>
	class SystemsInitializer final
	{
		friend SystemManager<T>;

	public:
		template <typename U>
		void DefineSystem() const;

	private:
		SystemManager<T>* _src = nullptr;
		StackAllocator* _allocator = nullptr;
		StackAllocator* _tempAllocator = nullptr;
		T const* _data = nullptr;
	};

	template <typename T>
	class Systems final
	{
		friend SystemManager<T>;

	public:
		template <typename U>
		[[nodiscard]] U* GetSystem();

		[[nodiscard]] Iterator<System<T>*> begin() const;
		[[nodiscard]] Iterator<System<T>*> end() const;

	private:
		SystemManager<T>* _src = nullptr;
	};

	/// <summary>
	/// Manages all systems that can interact with the engine.
	/// </summary>
	template <typename T>
	class SystemManager final
	{
		friend SystemsInitializer<T>;
		friend Systems<T>;

	public:
		[[nodiscard]] SystemsInitializer<T> CreateInitializer(StackAllocator& allocator, StackAllocator& tempAllocator, const T& data);

		void Allocate(StackAllocator& allocator, StackAllocator& tempAllocator);
		void Free(StackAllocator& allocator, StackAllocator& tempAllocator, const T& data);

		void Awake(const T& data);
		void Start(const T& data);
		void Update(const T& data);
		void OnRecreateSwapChainAssets(const T& data);
		void OnKeyInput(const T& data, int key, int action);
		void OnMouseInput(const T& data, int key, int action);
		void Exit(const T& data);

		[[nodiscard]] operator Systems<T>();

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

		template <typename U>
		void DefineSystem(StackAllocator& allocator, StackAllocator& tempAllocator, const T& data);
		template <typename U>
		[[nodiscard]] U* GetSystem();
	};

	template <typename T>
	SystemsInitializer<T> SystemManager<T>::CreateInitializer(StackAllocator& allocator, StackAllocator& tempAllocator,
		const T& data)
	{
		assert(!_allocated);
		assert(!_tempSystemData);
		_tempSystemData.Allocate(tempAllocator, 16);

		SystemsInitializer<T> systems{};
		systems._src = this;
		systems._allocator = &allocator;
		systems._tempAllocator = &tempAllocator;
		systems._data = &data;
		return systems;
	}

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
	void SystemManager<T>::Free(StackAllocator& allocator, StackAllocator& tempAllocator, const T& data)
	{
		_allocated = false;

		Array<System<T>*> systemsCpy{};
		systemsCpy.Allocate(tempAllocator, _vector.GetCount());
		Copy(systemsCpy.GetView(), 0, systemsCpy.GetLength(), _vector.GetData());

		_vector.Free(allocator);
		_map.Free(allocator);
		_allocations.Free(allocator);

		for (int32_t i = systemsCpy.GetLength() - 1; i >= 0; --i)
		{
			systemsCpy[i]->Free(data, *this);
			allocator.MFree(_allocations[i]);
		}

		systemsCpy.Free(tempAllocator);
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
	SystemManager<T>::operator Systems<T>()
	{
		Systems<T> systems{};
		systems._src = this;
		return systems;
	}

	template <typename T>
	template <typename U>
	void SystemsInitializer<T>::DefineSystem() const
	{
		return _src->template DefineSystem<U>(*_allocator, *_tempAllocator, *_data);
	}

	template <typename T>
	template <typename U>
	U* Systems<T>::GetSystem()
	{
		assert(_src->_allocated);
		return _src->template GetSystem<U>();
	}

	template <typename T>
	Iterator<System<T>*> Systems<T>::begin() const
	{
		return _src->_vector.begin();
	}

	template <typename T>
	Iterator<System<T>*> Systems<T>::end() const
	{
		return _src->_vector.end();
	}

	template <typename T>
	template <typename U>
	void SystemManager<T>::DefineSystem(StackAllocator& allocator, StackAllocator& tempAllocator, const T& data)
	{
		assert(!_allocated);

		auto allocation = allocator.New<U>();
		System<T>* basePtr = allocation.ptr;
		basePtr->Allocate(data, *this);

		TempSystemData tempSystemData{};
		tempSystemData.allocation = allocation.id;
		tempSystemData.ptr = allocation.ptr;
		tempSystemData.hashCode = typeid(U).hash_code();
		_tempSystemData.Add(tempSystemData, &tempAllocator, &allocator);
	}

	template <typename T>
	template <typename U>
	U* SystemManager<T>::GetSystem()
	{
		System<T>** ptr = _map.Contains(typeid(U).hash_code());
		return static_cast<U*>(*ptr);
	}
}
