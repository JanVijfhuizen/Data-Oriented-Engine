#pragma once
#include "Vector.h"

namespace jlb
{
	// Vector that will spawn internal nested vectors to store data in when the capacity has been exceeded.
	template <typename T>
	class NestedVector final
	{
	public:
		class Node final : public Vector<T>
		{
			friend NestedVector;
			
			Allocation<Node> _next{};
		};

		struct Iterator final
		{
			// Start of iteration.
			Node const* src = nullptr;
			size_t index = 0;

			[[nodiscard]] T& operator*() const;
			[[nodiscard]] T& operator->() const;

			const Iterator& operator++();
			Iterator operator++(int);

			friend bool operator==(const Iterator& a, const Iterator& b)
			{
				return a.src == b.src && a.index == b.index;
			}

			friend bool operator!= (const Iterator& a, const Iterator& b)
			{
				return !(a == b);
			}
		};

		[[nodiscard]] T& operator [](size_t index) const;
		
		void Allocate(StackAllocator& allocator, size_t size, size_t nestableCapacity, const T& fillValue = {});
		void Free(StackAllocator& allocator);

		// Detach nested without deallocating them.
		void DetachNested();
		// Free all the nested vectors.
		void RemoveNested(StackAllocator& allocator);
		// Sets the count to 0, as well as for all the nested vectors.
		void Clear();
		// Allocates a nested based on the capacity - root.capacity.
		void PreAllocateNested(StackAllocator& allocator, size_t capacity);

		T& Add(StackAllocator& allocator, T& value);
		T& Add(StackAllocator& allocator, const T& value = {});
		void RemoveAt(size_t index);

		[[nodiscard]] const Node& GetRoot() const;
		[[nodiscard]] size_t GetVectorCount() const;
		[[nodiscard]] size_t GetCount() const;
		[[nodiscard]] size_t GetLength() const;

		[[nodiscard]] Iterator begin() const;
		[[nodiscard]] Iterator end() const;

	private:
		Allocation<Node> _root{};
		size_t _nestableCapacity = 0;

		[[nodiscard]] T& IntAdd(StackAllocator& allocator, const T& value);
		void RemoveNested(StackAllocator& allocator, Node& node);
	};

	template <typename T>
	T& NestedVector<T>::Iterator::operator*() const
	{
		assert(src);
		return src->operator[](index);
	}

	template <typename T>
	T& NestedVector<T>::Iterator::operator->() const
	{
		assert(src);
		return src->operator[](index);
	}

	template <typename T>
	const typename NestedVector<T>::Iterator& NestedVector<T>::Iterator::operator++()
	{
		++index;
		const bool next = index >= src->GetCount();
		if(next && src->_next)
		{
			src = src->_next;
			index = 0;
		}
		return *this;
	}

	template <typename T>
	typename NestedVector<T>::Iterator NestedVector<T>::Iterator::operator++(int)
	{
		Iterator temp(src, index);
		++index;
		const bool next = index >= src->GetCount();
		src = next ? src->_next : src;
		index = next ? 0 : index;
		return temp;
	}

	template <typename T>
	T& NestedVector<T>::operator[](size_t index) const
	{
		Node* node = _root;
		while (node)
		{
			const size_t length = node->GetLength();

			if (index < length)
				return node->operator[](index);
			index -= length;
			
			node = node->_next;
		}

		assert(false);
	}

	template <typename T>
	void NestedVector<T>::Allocate(StackAllocator& allocator, 
		const size_t size, const size_t nestableCapacity,
		const T& fillValue)
	{
		assert(!_root);
		_root = allocator.New<Node>();
		_root.ptr->Allocate(allocator, size, fillValue);
		_nestableCapacity = nestableCapacity;
	}

	template <typename T>
	void NestedVector<T>::Free(StackAllocator& allocator)
	{
		RemoveNested(allocator);
		_root.ptr->Free(allocator);
		allocator.MFree(_root.id);
		_root = {};
	}

	template <typename T>
	void NestedVector<T>::DetachNested()
	{
		_root.ptr->_next = {};
	}

	template <typename T>
	void NestedVector<T>::RemoveNested(StackAllocator& allocator)
	{
		RemoveNested(allocator, *_root.ptr);
	}

	template <typename T>
	void NestedVector<T>::Clear()
	{
		Node* node = _root;
		while (node)
		{
			node->SetCount(0);
			node = node->_next;
		}
	}

	template <typename T>
	void NestedVector<T>::PreAllocateNested(StackAllocator& allocator, size_t capacity)
	{
		Node* node = _root;
		while (node)
		{
			const size_t length = node->GetLength();
			capacity = capacity > length ? capacity - length : 0;

			if (!node->_next && capacity > 0)
			{
				assert(_nestableCapacity > 0);
				auto newNode = node->_next = allocator.New<Node>();
				newNode.ptr->Allocate(allocator, _nestableCapacity);
			}

			node = node->_next;
		}
	}

	template <typename T>
	T& NestedVector<T>::Add(StackAllocator& allocator, T& value)
	{
		return IntAdd(allocator, value);
	}

	template <typename T>
	T& NestedVector<T>::Add(StackAllocator& allocator, const T& value)
	{
		return IntAdd(allocator, value);
	}

	template <typename T>
	void NestedVector<T>::RemoveAt(size_t index)
	{
		assert(index < GetCount());

		Node* node = _root;
		while (node)
		{
			const size_t length = node->GetLength();
			if (index < length)
			{
				node->RemoveAt(index);
				break;
			}
				
			node = node->_next;
			index -= length;
		}
	}

	template <typename T>
	const typename NestedVector<T>::Node& NestedVector<T>::GetRoot() const
	{
		return *_root;
	}

	template <typename T>
	size_t NestedVector<T>::GetVectorCount() const
	{
		size_t count = 0;
		Node* node = _root;
		while (node)
		{
			++count;
			node = node->_next;
		}

		return count;
	}

	template <typename T>
	size_t NestedVector<T>::GetCount() const
	{
		size_t count = 0;
		Node* node = _root;
		while (node)
		{
			count += node->GetCount();
			node = node->_next;
		}

		return count;
	}

	template <typename T>
	size_t NestedVector<T>::GetLength() const
	{
		size_t count = 0;
		Node* node = _root;
		while (node)
		{
			count += node->GetLength();
			node = node->_next;
		}

		return count;
	}

	template <typename T>
	typename NestedVector<T>::Iterator NestedVector<T>::begin() const
	{
		Iterator iterator{};
		iterator.src = _root;
		return iterator;
	}

	template <typename T>
	typename NestedVector<T>::Iterator NestedVector<T>::end() const
	{
		Iterator iterator{};

		Node* node = _root;
		while (node)
		{
			const size_t count = node->GetCount();
			const bool full = count == node->GetLength();
			if(!full)
			{
				iterator.index = count;
				break;
			}

			node = node->_next;
		}

		iterator.src = node;
		return iterator;
	}

	template <typename T>
	T& NestedVector<T>::IntAdd(StackAllocator& allocator, const T& value)
	{
		Node* node = _root;
		while (node)
		{
			const size_t length = node->GetLength();
			const size_t count = node->GetCount();

			if(length == count)
			{
				if(!node->_next)
				{
					assert(_nestableCapacity > 0);
					auto newNode = node->_next = allocator.New<Node>();
					newNode.ptr->Allocate(allocator, _nestableCapacity);
					return newNode.ptr->Add(value);
				}

				node = node->_next;
				continue;
			}

			return node->Add(value);
		}

		assert(false);
	}

	template <typename T>
	void NestedVector<T>::RemoveNested(StackAllocator& allocator, Node& node)
	{
		if (!node._next)
			return;
		RemoveNested(allocator, *node._next.ptr);
		node._next.ptr->Free(allocator);
		allocator.MFree(node._next.id);
		node._next = {};
	}
}
