#pragma once
#include "Vector.h"

namespace jlb
{
	template <typename T>
	class SparseSet final
	{
	public:
		struct Node final
		{
			T instance;
			size_t sparseIndex;
		};

		[[nodiscard]] Node& operator[](size_t index) const;

		void Allocate(StackAllocator& allocator, size_t capacity);
		void Free(StackAllocator& allocator);

		Node& Insert(size_t index, const T& instance = {});
		void RemoveAt(size_t index);
		void Clear();

		[[nodiscard]] bool Contains(size_t index);
		[[nodiscard]] size_t GetCount() const;
		[[nodiscard]] size_t GetLength() const;
		[[nodiscard]] ArrayView<Node> GetView() const;

		[[nodiscard]] Iterator<Node> begin() const;
		[[nodiscard]] Iterator<Node> end() const;

	private:
		Vector<Node> _instances{};
		Array<size_t> _sparse{};
	};

	template <typename T>
	typename SparseSet<T>::Node& SparseSet<T>::operator[](const size_t index) const
	{
		return _instances[_sparse[index]];
	}

	template <typename T>
	void SparseSet<T>::Allocate(StackAllocator& allocator, const size_t capacity)
	{
		_instances.Allocate(allocator, capacity);
		_sparse.Allocate(allocator, capacity, SIZE_MAX);
	}

	template <typename T>
	void SparseSet<T>::Free(StackAllocator& allocator)
	{
		_sparse.Free(allocator);
		_instances.Free(allocator);
	}

	template <typename T>
	typename SparseSet<T>::Node& SparseSet<T>::Insert(const size_t index, const T& instance)
	{
		_sparse[index] = _instances.GetCount();
		return _instances.Add({ instance, index });
	}

	template <typename T>
	void SparseSet<T>::RemoveAt(const size_t index)
	{
		auto& sparse = _sparse[index];
		_instances.RemoveAt(sparse);
		auto& node = _instances[sparse];
		_sparse[node.sparseIndex] = index;
		sparse = SIZE_MAX;
	}

	template <typename T>
	void SparseSet<T>::Clear()
	{
		for (auto& instance : _instances)
			_sparse[instance.sparseIndex] = SIZE_MAX;
		_instances.SetCount(0);
	}

	template <typename T>
	bool SparseSet<T>::Contains(const size_t index)
	{
		return _sparse[index] != SIZE_MAX;
	}

	template <typename T>
	size_t SparseSet<T>::GetCount() const
	{
		return _instances.GetCount();
	}

	template <typename T>
	size_t SparseSet<T>::GetLength() const
	{
		return _instances.GetLength();
	}

	template <typename T>
	ArrayView<typename SparseSet<T>::Node> SparseSet<T>::GetView() const
	{
		return _instances.GetView();
	}

	template <typename T>
	Iterator<typename SparseSet<T>::Node> SparseSet<T>::begin() const
	{
		return _instances.begin();
	}

	template <typename T>
	Iterator<typename SparseSet<T>::Node> SparseSet<T>::end() const
	{
		return _instances.end();
	}
}
