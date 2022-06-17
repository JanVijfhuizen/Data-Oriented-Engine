#pragma once
#include "StackAllocator.h"
#include <EngineData.h>
#include <vcruntime_typeinfo.h>

namespace game
{
	class SystemChain;

	class ISystemChainable
	{
		friend SystemChain;

	protected:
		virtual void Allocate(const EngineData& EngineData, SystemChain& chain) = 0;
		virtual void Free(const EngineData& EngineData, SystemChain& chain) = 0;

		virtual void Awake(const EngineData& EngineData, SystemChain& chain);
		virtual void Start(const EngineData& EngineData, SystemChain& chain);
		virtual void Update(const EngineData& EngineData, SystemChain& chain) = 0;

		virtual void CreateSwapChainAssets(const EngineData& EngineData, SystemChain& chain);
		virtual void DestroySwapChainAssets(const EngineData& EngineData, SystemChain& chain);

	private:
		size_t* _src = nullptr;
		ISystemChainable* _previous = nullptr;
		ISystemChainable* _next = nullptr;
		const char* _typeName = nullptr;
	};

	class SystemChain final
	{
	public:
		class Iterator final
		{
		public:
			Iterator();
			Iterator(ISystemChainable* chainable);

			ISystemChainable& operator*() const;
			ISystemChainable& operator->() const;

			const Iterator& operator++();
			Iterator operator++(int);

			friend bool operator==(const Iterator& a, const Iterator& b)
			{
				return a._ptr == b._ptr;
			};

			friend bool operator!= (const Iterator& a, const Iterator& b)
			{
				return !(a == b);
			}

		private:
			ISystemChainable* _ptr = nullptr;
		};

		template <typename T>
		void Add(const EngineData& EngineData);

		template <typename T>
		[[nodiscard]] T* Get();

		void Allocate(const EngineData& EngineData);
		void Free(const EngineData& EngineData);

		void Awake(const EngineData& EngineData);
		void Start(const EngineData& EngineData);
		void Update(const EngineData& EngineData);

		void RecreateSwapChainAssets(const EngineData& EngineData);

		[[nodiscard]] Iterator begin() const;
		[[nodiscard]] Iterator end();

	private:
		ISystemChainable* _head = nullptr;

		void ReverseExecute(void (ISystemChainable::* ptr)(const EngineData&, SystemChain&), const EngineData& EngineData);
	};

	template <typename T>
	void SystemChain::Add(const EngineData& EngineData)
	{
		auto allocation = EngineData.allocator->New<T>();
		ISystemChainable* ptr = static_cast<ISystemChainable*>(allocation.ptr);
		ptr->_src = allocation.id.src;
		ptr->_typeName = typeid(T).name();
		if(_head)
			_head->_previous = ptr;
		ptr->_next = _head;
		_head = ptr;
	}

	template <typename T>
	T* SystemChain::Get()
	{
		const char* name = typeid(T).name();
		for (auto& chainable : *this)
		{
			if (name == chainable._typeName)
				return static_cast<T*>(&chainable);
		}

		return nullptr;
	}
}
