#pragma once
#include "Vector.h"

namespace jlb
{
	// Vector that will spawn internal nested vectors to store data in when the capacity has been exceeded.
	template <typename T>
	class NestableVector final
	{
	public:
		class Node final : public Vector<T>
		{
			friend NestableVector;
			
			Allocation<Node> _next{};
		};

		struct Iterator final
		{
			// Start of iteration.
			Node const* src = nullptr;

			const Node& operator*() const;
			const Node& operator->() const;

			const Iterator& operator++();
			Iterator operator++(int);

			friend bool operator==(const Iterator& a, const Iterator& b)
			{
				return a.src == b.src;
			}

			friend bool operator!= (const Iterator& a, const Iterator& b)
			{
				return !(a == b);
			}
		};
		
		void Allocate(StackAllocator& allocator, size_t size, size_t nestableCapacity, const T& fillValue = {});
		void Free(StackAllocator& allocator);

		// Free all the nested vectors.
		void RemoveNested(StackAllocator& allocator);
		// Sets the count to 0, as well as for all the nested vectors.
		void Clear();

		T& Add(StackAllocator& allocator, T& value);
		T& Add(StackAllocator& allocator, const T& value = {});

		[[nodiscard]] const Node& GetRoot() const;
		[[nodiscard]] size_t GetVectorCount() const;
		[[nodiscard]] size_t GetCount() const;
		[[nodiscard]] size_t GetLength() const;

		[[nodiscard]] Iterator begin() const;
		[[nodiscard]] Iterator end() const;

	private:
		Allocation<Node> _top{};
		size_t _nestableCapacity = 0;
		Node* _root = nullptr;

		[[nodiscard]] T& IntAdd(StackAllocator& allocator, const T& value);
	};

	template <typename T>
	const typename NestableVector<T>::Node& NestableVector<T>::Iterator::operator*() const
	{
		assert(src);
		return *src;
	}

	template <typename T>
	const typename NestableVector<T>::Node& NestableVector<T>::Iterator::operator->() const
	{
		assert(src);
		return *src;
	}

	template <typename T>
	const typename NestableVector<T>::Iterator& NestableVector<T>::Iterator::operator++()
	{
		src = src->_next;
		return *this;
	}

	template <typename T>
	typename NestableVector<T>::Iterator NestableVector<T>::Iterator::operator++(int)
	{
		Iterator temp(src);
		temp.src = temp.src->_next;
		return temp;
	}

	template <typename T>
	void NestableVector<T>::Allocate(StackAllocator& allocator, 
		const size_t size, const size_t nestableCapacity,
		const T& fillValue)
	{
		assert(!_top);
		_top = allocator.New<Node>();
		_top.ptr->Allocate(allocator, size, fillValue);
		_nestableCapacity = nestableCapacity;
	}

	template <typename T>
	void NestableVector<T>::Free(StackAllocator& allocator)
	{
		RemoveNested(allocator);
		allocator.MFree(_top.id);
		_top = {};
	}

	template <typename T>
	void NestableVector<T>::RemoveNested(StackAllocator& allocator)
	{
		Node* nested = _top.ptr->_next;
		while(nested)
		{
			Node* next = nested->_next;
			nested->Free(allocator);
			nested = next;
		}
	}

	template <typename T>
	void NestableVector<T>::Clear()
	{
		Node* node = _top;
		while (node)
		{
			node->SetCount(0);
			node = node->_next;
		}
	}

	template <typename T>
	T& NestableVector<T>::Add(StackAllocator& allocator, T& value)
	{
		return IntAdd(allocator, value);
	}

	template <typename T>
	T& NestableVector<T>::Add(StackAllocator& allocator, const T& value)
	{
		return IntAdd(allocator, value);
	}

	template <typename T>
	const typename NestableVector<T>::Node& NestableVector<T>::GetRoot() const
	{
		return *_root;
	}

	template <typename T>
	size_t NestableVector<T>::GetVectorCount() const
	{
		size_t count = 0;
		Node* node = _top;
		while (node)
		{
			++count;
			node = node->_next;
		}

		return count;
	}

	template <typename T>
	size_t NestableVector<T>::GetCount() const
	{
		size_t count = 0;
		Node* node = _top;
		while (node)
		{
			count += node->GetCount();
			node = node->_next;
		}

		return count;
	}

	template <typename T>
	size_t NestableVector<T>::GetLength() const
	{
		size_t count = 0;
		Node* node = _top;
		while (node)
		{
			count += node->GetLength();
			node = node->_next;
		}

		return count;
	}

	template <typename T>
	typename NestableVector<T>::Iterator NestableVector<T>::begin() const
	{
		Iterator iterator{};
		iterator.src = _top;
		return iterator;
	}

	template <typename T>
	typename NestableVector<T>::Iterator NestableVector<T>::end() const
	{
		Iterator iterator{};
		iterator.src = nullptr;
		return iterator;
	}

	template <typename T>
	T& NestableVector<T>::IntAdd(StackAllocator& allocator, const T& value)
	{
		Node* node = _top;
		while (node)
		{
			const size_t length = node->GetLength();
			const size_t count = node->GetCount();

			if(length == count)
			{
				if(!node->_next)
				{
					assert(_nestableCapacity > 0);
					const auto oldRoot = _top;
					_top = allocator.New<Node>();
					_top.ptr->Allocate(allocator, _nestableCapacity);
					_top.ptr->_next = oldRoot;
					return _top.ptr->Add(value);
				}

				node = node->_next;
				continue;
			}

			return node->Add(value);
		}

		assert(false);
	}
}
