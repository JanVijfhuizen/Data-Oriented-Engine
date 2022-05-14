#include "pch.h"
#include "SystemChain.h"

namespace game
{
	SystemChain::Iterator::Iterator(ISystemChainable* chainable) : _ptr(chainable)
	{

	}

	SystemChain::Iterator& SystemChain::Iterator::operator=(ISystemChainable* chainable)
	{
		this->_ptr = chainable;
		return *this;
	}

	SystemChain::Iterator& SystemChain::Iterator::operator++()
	{
		if (_ptr)
			_ptr = _ptr->_next;
		return *this;
	}

	bool SystemChain::Iterator::operator!=(const Iterator& iterator) const
	{
		return _ptr != iterator._ptr;
	}

	ISystemChainable& SystemChain::Iterator::operator*() const
	{
		return *_ptr;
	}

	ISystemChainable& SystemChain::Iterator::operator->() const
	{
		return *_ptr;
	}

	void SystemChain::Allocate(const EngineOutData& outData)
	{
		ReverseExecute(&ISystemChainable::Allocate, outData);
	}

	void SystemChain::Free(const EngineOutData& outData)
	{
		for (auto& chainable : *this)
			chainable.Free(outData, *this);

		ISystemChainable* current = _head;
		while (current)
		{
			ISystemChainable* next = current->_next;
			outData.allocator->MFreeUnsafe(next);
			current = next;
		}
	}

	void SystemChain::Start(const EngineOutData& outData)
	{
		ReverseExecute(&ISystemChainable::Start, outData);
	}

	void SystemChain::Update(const EngineOutData& outData)
	{
		ReverseExecute(&ISystemChainable::Update, outData);
	}

	SystemChain::Iterator SystemChain::begin() const
	{
		return _head;
	}

	SystemChain::Iterator SystemChain::end()
	{
		return nullptr;
	}

	void SystemChain::ReverseExecute(void(ISystemChainable::* ptr)(const EngineOutData&, SystemChain&), const EngineOutData& outData)
	{
		ISystemChainable* current = _head;
		while (current)
			current = current->_next;
		while (current)
		{
			(current->*ptr)(outData, *this);
			current = current->_previous;
		}
	}
}