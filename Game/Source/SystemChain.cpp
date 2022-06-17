#include "pch.h"
#include "SystemChain.h"

namespace game
{
	void ISystemChainable::Awake(const EngineData& EngineData, SystemChain& chain)
	{
	}

	void ISystemChainable::Start(const EngineData& EngineData, SystemChain& chain)
	{
	}

	void ISystemChainable::CreateSwapChainAssets(const EngineData& EngineData, SystemChain& chain)
	{
	}

	void ISystemChainable::DestroySwapChainAssets(const EngineData& EngineData, SystemChain& chain)
	{
	}

	SystemChain::Iterator::Iterator() = default;

	SystemChain::Iterator::Iterator(ISystemChainable* chainable) : _ptr(chainable)
	{

	}

	const SystemChain::Iterator& SystemChain::Iterator::operator++()
	{
		if (_ptr)
			_ptr = _ptr->_next;
		return *this;
	}

	SystemChain::Iterator SystemChain::Iterator::operator++(int)
	{
		if (_ptr)
			_ptr = _ptr->_next;
		return *this;
	}

	ISystemChainable& SystemChain::Iterator::operator*() const
	{
		return *_ptr;
	}

	ISystemChainable& SystemChain::Iterator::operator->() const
	{
		return *_ptr;
	}

	void SystemChain::Allocate(const EngineData& EngineData)
	{
		ReverseExecute(&ISystemChainable::Allocate, EngineData);
	}

	void SystemChain::Free(const EngineData& EngineData)
	{
		for (auto& chainable : *this)
			chainable.DestroySwapChainAssets(EngineData, *this);

		for (auto& chainable : *this)
			chainable.Free(EngineData, *this);

		ISystemChainable* current = _head;
		while (current)
		{
			ISystemChainable* next = current->_next;
			EngineData.allocator->MFreeUnsafe(current->_src);
			current = next;
		}
	}

	void SystemChain::Awake(const EngineData& EngineData)
	{
		ReverseExecute(&ISystemChainable::CreateSwapChainAssets, EngineData);
		ReverseExecute(&ISystemChainable::Awake, EngineData);
	}

	void SystemChain::Start(const EngineData& EngineData)
	{
		ReverseExecute(&ISystemChainable::Start, EngineData);
	}

	void SystemChain::Update(const EngineData& EngineData)
	{
		ReverseExecute(&ISystemChainable::Update, EngineData);
	}

	void SystemChain::RecreateSwapChainAssets(const EngineData& EngineData)
	{
		for (auto& chain : *this)
		{
			chain.DestroySwapChainAssets(EngineData, *this);
			chain.CreateSwapChainAssets(EngineData, *this);
		}
	}

	SystemChain::Iterator SystemChain::begin() const
	{
		return _head;
	}

	SystemChain::Iterator SystemChain::end()
	{
		return nullptr;
	}

	void SystemChain::ReverseExecute(void(ISystemChainable::* ptr)(const EngineData&, SystemChain&), const EngineData& EngineData)
	{
		ISystemChainable* current = _head;
		while (true)
		{
			if (!current->_next)
				break;
			current = current->_next;
		}
		while (current)
		{
			(current->*ptr)(EngineData, *this);
			current = current->_previous;
		}
	}
}