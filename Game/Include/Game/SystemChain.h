#pragma once
#include "StackAllocator.h"
#include <EngineData.h>

namespace game
{
	class SystemChain;

	class ISystemChainable
	{
		friend SystemChain;

	protected:
		virtual void Allocate(const EngineOutData& outData, SystemChain& chain) = 0;
		virtual void Free(const EngineOutData& outData, SystemChain& chain) = 0;

		virtual void Start(const EngineOutData& outData, SystemChain& chain) {};
		virtual void Update(const EngineOutData& outData, SystemChain& chain) = 0;

	private:
		ISystemChainable* _previous = nullptr;
		ISystemChainable* _next = nullptr;
	};

	class SystemChain final
	{
	public:
		class Iterator final
		{
		public:
			Iterator() = default;;
			Iterator(ISystemChainable* chainable);

			Iterator& operator=(ISystemChainable* chainable);
			Iterator& operator++();
			bool operator!=(const Iterator& iterator) const;

			ISystemChainable& operator*() const;
			ISystemChainable& operator->() const;

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
		[[nodiscard]] static Iterator end();

	private:
		ISystemChainable* _head = nullptr;

		void ReverseExecute(void (ISystemChainable::* ptr)(const EngineOutData&, SystemChain&), const EngineOutData& outData);
	};

	template <typename T>
	void SystemChain::Add(const EngineOutData& outData)
	{
		auto allocation = outData.allocator->New<T>();
		ISystemChainable* ptr = static_cast<ISystemChainable*>(allocation.ptr);
		_head->_previous = ptr;
		allocation.ptr->_next = _head;
		_head = ptr;
	}

	template <typename T>
	T* SystemChain::Get()
	{
		for (auto& chainable : *this)
		{
			T* cast = dynamic_cast<T*>(&chainable);
			if (cast)
				return cast;
		}

		return nullptr;
	}
}
