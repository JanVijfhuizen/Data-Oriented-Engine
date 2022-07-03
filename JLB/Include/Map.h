#pragma once
#include "Array.h"
#include "KeyPair.h"

namespace jlb
{
	// Data container that that prioritizes quick lookup speed.
	template <typename T>
	class Map
	{
	public:
		virtual void Allocate(StackAllocator& allocator, size_t size);
		virtual void Free(StackAllocator& allocator);

		// Duplicates are not inserted.
		void Insert(T& value, size_t key);
		// Duplicates are not inserted.
		void Insert(T&& value, size_t key);

		[[nodiscard]] T* Contains(size_t key) const;
		void Erase(T& value);
		[[nodiscard]] size_t GetCount() const;

	protected:
		[[nodiscard]] size_t GetHash(size_t key) const;
		void _Insert(T& value, size_t key);

	private:
		size_t _count = 0;
		Array<KeyPair<T>> _array{};
	};

	template <typename T>
	void Map<T>::Allocate(StackAllocator& allocator, const size_t size)
	{
		_array.Allocate(allocator, size);
	}

	template <typename T>
	void Map<T>::Free(StackAllocator& allocator)
	{
		_array.Free(allocator);
		_count = 0;
	}

	template <typename T>
	void Map<T>::Insert(T& value, const size_t key)
	{
		_Insert(value, key);
	}

	template <typename T>
	void Map<T>::Insert(T&& value, const size_t key)
	{
		_Insert(value, key);
	}

	template <typename T>
	void Map<T>::Erase(T& value)
	{
		size_t index;
		const bool contains = Contains(value, index);
		assert(contains);

		const size_t length = _array.GetLength();
		assert(_count > 0);

		auto& keyPair = _array[index];

		// Check how big the key group is.
		size_t i = 1;
		while (i < length)
		{
			const size_t otherIndex = (index + i) % length;
			auto& otherKeyPair = _array[otherIndex];
			if (otherKeyPair.key != keyPair.key)
				break;
			++i;
		}

		// Setting the keypair value to the default value.
		keyPair = {};
		// Move the key group one place backwards by swapping the first and last index.
		Swap(_array, index, index + i - 1);
		--_count;
	}

	template <typename T>
	T* Map<T>::Contains(const size_t key) const
	{
		const size_t length = _array.GetLength();
		assert(_count < length);

		// Get and use the hash as an index.
		const size_t hash = GetHash(key);

		for (size_t i = 0; i < length; ++i)
		{
			const size_t index = (hash + i) % length;
			auto& keyPair = _array[index];

			// If the hash is different, continue.
			if (keyPair.key == key)
				return &keyPair.value;
		}

		return nullptr;
	}

	template <typename T>
	size_t Map<T>::GetCount() const
	{
		return _count;
	}

	template <typename T>
	size_t Map<T>::GetHash(const size_t key) const
	{
		return key % _array.GetLength();
	}

	template <typename T>
	void Map<T>::_Insert(T& value, const size_t key)
	{
		const size_t length = _array.GetLength();
		assert(_count < length);

		// If it already contains this value, replace the old one with the newer value.
		if (Contains(key))
			return;

		const size_t hash = GetHash(key);

		for (size_t i = 0; i < length; ++i)
		{
			const size_t index = (hash + i) % length;
			auto& keyPair = _array[index];
			// Set to true the first time the key group has been found.
			if (keyPair.key != SIZE_MAX)
				continue;

			keyPair.key = key;
			keyPair.value = value;
			++_count;
			break;
		}
	}
}
