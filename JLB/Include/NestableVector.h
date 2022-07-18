#pragma once
#include "Vector.h"

namespace jlb
{
	// Vector that will spawn internal nested vectors to store data in when the capacity has been exceeded.
	template <typename T>
	class NestableVector : public Vector<T>
	{
	public:
		struct Iterator final
		{
			// Start of iteration.
			NestableVector<T>* src = nullptr;
			// Length of iteration.
			size_t length = 0;
			// Current index of iteration (relative to begin).
			size_t index = 0;

			T& operator*() const;
			T& operator->() const;

			const Iterator& operator++();
			Iterator operator++(int);

			friend bool operator==(const Iterator& a, const Iterator& b)
			{
				return a.index == b.index;
			}

			friend bool operator!= (const Iterator& a, const Iterator& b)
			{
				return !(a == b);
			}
		};

		T& operator[](size_t index) const override;

		void Free(StackAllocator& allocator) override;
		// Free all the nested vectors.
		void RemoveNested(StackAllocator& allocator);
		// Sets the count to 0, as well as for all the nested vectors.
		void Clear();

		T& Add(StackAllocator& allocator, T& value);
		T& Add(StackAllocator& allocator, const T& value = {});

		[[nodiscard]] size_t GetLength() const override;
		[[nodiscard]] size_t GetCount() const override;

		[[nodiscard]] Iterator begin();
		[[nodiscard]] Iterator end();

	private:
		using Vector<T>::Add;
		using Vector<T>::RemoveAt;
		using Vector<T>::GetView;
		using Vector<T>::begin;
		using Vector<T>::end;
		using Vector<T>::SetCount;

		Allocation<NestableVector<T>> _next{};

		[[nodiscard]] T& IntAdd(StackAllocator& allocator, const T& value);
	};

	template <typename T>
	T& NestableVector<T>::Iterator::operator*() const
	{
		assert(src);
		assert(index <= length);
		return src->operator[](index);
	}

	template <typename T>
	T& NestableVector<T>::Iterator::operator->() const
	{
		assert(src);
		assert(index <= length);
		return src->operator[](index);
	}

	template <typename T>
	const typename NestableVector<T>::Iterator& NestableVector<T>::Iterator::operator++()
	{
		++index;
		return *this;
	}

	template <typename T>
	typename NestableVector<T>::Iterator NestableVector<T>::Iterator::operator++(int)
	{
		Iterator temp(src, length, index);
		++index;
		return temp;
	}

	template <typename T>
	T& NestableVector<T>::operator[](const size_t index) const
	{
		const size_t length = Vector<T>::GetLength();
		if(index >= length)
		{
			assert(_next);
			return _next.ptr->operator[](index - length);
		}

		return Vector<T>::operator[](index);
	}

	template <typename T>
	void NestableVector<T>::Free(StackAllocator& allocator)
	{
		RemoveNested(allocator);
		Vector<T>::Free(allocator);
	}

	template <typename T>
	void NestableVector<T>::RemoveNested(StackAllocator& allocator)
	{
		if (!_next)
			return;
		_next.ptr->RemoveNested(allocator);
		allocator.MFree(_next.id);
		_next = {};
	}

	template <typename T>
	void NestableVector<T>::Clear()
	{
		SetCount(0);
		if (_next)
			_next.ptr->Clear();
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
	size_t NestableVector<T>::GetLength() const
	{
		return Vector<T>::GetLength() + (_next ? _next.ptr->GetLength() : 0);
	}

	template <typename T>
	size_t NestableVector<T>::GetCount() const
	{
		return Vector<T>::GetCount() + (_next ? _next.ptr->GetCount() : 0);
	}

	template <typename T>
	typename NestableVector<T>::Iterator NestableVector<T>::begin()
	{
		Iterator iterator{};
		iterator.src = this;
		iterator.length = GetCount();
		iterator.index = 0;
		return iterator;
	}

	template <typename T>
	typename NestableVector<T>::Iterator NestableVector<T>::end()
	{
		Iterator iterator{};
		iterator.src = this;
		iterator.length = GetCount();
		iterator.index = GetCount();
		return iterator;
	}

	template <typename T>
	T& NestableVector<T>::IntAdd(StackAllocator& allocator, const T& value)
	{
		const size_t length = Vector<T>::GetLength();
		const size_t count = Vector<T>::GetCount();

		if(count >= length)
		{
			if (!_next)
			{
				assert(length > 0);
				_next = allocator.New<NestableVector<T>>();
				_next.ptr->Allocate(allocator, Vector<T>::GetLength());
			}
			return _next.ptr->IntAdd(allocator, value);
		}

		return Vector<T>::Add(value);
	}
}
