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
		void RemoveNested(StackAllocator* allocator);

		[[nodiscard]] size_t GetLength() const override;

	protected:
		bool TryExpand(const T& value, StackAllocator* allocator, StackAllocator* tempAllocator) override;

	private:
		using Vector<T>::GetView;
		using Vector<T>::begin;
		using Vector<T>::end;

		Allocation<NestableVector<T>> _next = nullptr;
	};

	template <typename T>
	T& NestableVector<T>::Iterator::operator*() const
	{
		assert(src);
		assert(index <= length);
		return src[index];
	}

	template <typename T>
	T& NestableVector<T>::Iterator::operator->() const
	{
		assert(src);
		assert(index <= length);
		return src[index];
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
		auto target = index >= length ? _next : this;
		assert(target);
		return target->operator[](index - length);
	}

	template <typename T>
	void NestableVector<T>::Free(StackAllocator& allocator)
	{
		RemoveNested(allocator);
		Vector<T>::Free(allocator);
	}

	template <typename T>
	void NestableVector<T>::RemoveNested(StackAllocator* allocator)
	{
		if (!_next)
			return;
		_next.ptr->RemoveNested(allocator);
		allocator->MFree(_next);
		_next = nullptr;
	}

	template <typename T>
	size_t NestableVector<T>::GetLength() const
	{
		return Vector<T>::GetLength() + (_next ? _next.ptr->GetLength() : 0);
	}

	template <typename T>
	bool NestableVector<T>::TryExpand(const T& value, StackAllocator* allocator, StackAllocator* tempAllocator)
	{
		if(!_next)
		{
			assert(Vector<T>::GetLength() > 0);
			_next = allocator->New<NestableVector<T>>();
			_next.ptr->Allocate(allocator, Vector<T>::GetLength());
		}
		_next->_Add(value, allocator, tempAllocator);
		return true;
	}
}
