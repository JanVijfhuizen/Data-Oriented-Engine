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

	public:
		virtual void Allocate(const EngineOutData& outData, SystemChain& chain) = 0;
		virtual void Free(const EngineOutData& outData, SystemChain& chain) = 0;

		virtual void Start(const EngineOutData& outData, SystemChain& chain) {};
		virtual void Update(const EngineOutData& outData, SystemChain& chain) = 0;

	private:
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
		void Add(const EngineOutData& outData);

		template <typename T>
		[[nodiscard]] T* Get();

		void Allocate(const EngineOutData& outData);
		void Free(const EngineOutData& outData);

		void Start(const EngineOutData& outData);
		void Update(const EngineOutData& outData);

		[[nodiscard]] Iterator begin() const;
		[[nodiscard]] Iterator end();

	private:
		ISystemChainable* _head = nullptr;

		void ReverseExecute(void (ISystemChainable::* ptr)(const EngineOutData&, SystemChain&), const EngineOutData& outData);
	};

	template <typename T>
	void SystemChain::Add(const EngineOutData& outData)
	{
		auto allocation = outData.allocator->New<T>();
		ISystemChainable* ptr = static_cast<ISystemChainable*>(allocation.ptr);
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
