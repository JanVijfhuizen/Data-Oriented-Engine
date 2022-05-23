#include "pch.h"
#include "SystemChain.h"

namespace game
{
	void ISystemChainable::Awake(const EngineOutData& outData, SystemChain& chain)
	{
	}

	void ISystemChainable::Start(const EngineOutData& outData, SystemChain& chain)
	{
	}

	void ISystemChainable::CreateSwapChainAssets(const EngineOutData& outData, SystemChain& chain)
	{
	}

	void ISystemChainable::DestroySwapChainAssets(const EngineOutData& outData, SystemChain& chain)
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

	void SystemChain::Allocate(const EngineOutData& outData)
	{
		ReverseExecute(&ISystemChainable::Allocate, outData);
	}

	void SystemChain::Free(const EngineOutData& outData)
	{
		for (auto& chainable : *this)
			chainable.DestroySwapChainAssets(outData, *this);

		for (auto& chainable : *this)
			chainable.Free(outData, *this);

		ISystemChainable* current = _head;
		while (current)
		{
			ISystemChainable* next = current->_next;
			outData.allocator->MFreeUnsafe(current->_src);
			current = next;
		}
	}

	void SystemChain::Awake(const EngineOutData& outData)
	{
		ReverseExecute(&ISystemChainable::CreateSwapChainAssets, outData);
		ReverseExecute(&ISystemChainable::Awake, outData);
	}

	void SystemChain::Start(const EngineOutData& outData)
	{
		ReverseExecute(&ISystemChainable::Start, outData);
	}

	void SystemChain::Update(const EngineOutData& outData)
	{
		ReverseExecute(&ISystemChainable::Update, outData);
	}

	void SystemChain::RecreateSwapChainAssets(const EngineOutData& outData)
	{
		for (auto& chain : *this)
		{
			chain.DestroySwapChainAssets(outData, *this);
			chain.CreateSwapChainAssets(outData, *this);
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

	void SystemChain::ReverseExecute(void(ISystemChainable::* ptr)(const EngineOutData&, SystemChain&), const EngineOutData& outData)
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
			(current->*ptr)(outData, *this);
			current = current->_previous;
		}
	}
}