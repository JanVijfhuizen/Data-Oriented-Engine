#pragma once
#include "Array.h"
#include "KeyPair.h"

namespace jlb
{
	/// <summary>
	/// Data container that that prioritizes quick lookup speed.
	/// </summary>
	template <typename T>
	class HashMap
	{
	public:
		// Function used to get a hash value from a value.
		size_t(*hasher)(T& value);

		virtual void Allocate(StackAllocator& allocator, size_t size);
		virtual void Free(StackAllocator& allocator);

		// Duplicates are not inserted.
		void Insert(T& value);
		// Duplicates are not inserted.
		void Insert(T&& value);

		[[nodiscard]] bool Contains(T& value);
		void Erase(T& value);
		[[nodiscard]] size_t GetCount() const;

	protected:
		[[nodiscard]] size_t GetHash(T& value);
		[[nodiscard]] bool Contains(T& value, size_t& outIndex);
		void _Insert(T& value);

	private:
		size_t _count = 0;
		Array<KeyPair<T>> _array{};
	};

	template <typename T>
	void HashMap<T>::Allocate(StackAllocator& allocator, const size_t size)
	{
		_array.Allocate(allocator, size);
	}

	template <typename T>
	void HashMap<T>::Free(StackAllocator& allocator)
	{
		_array.Free(allocator);
		_count = 0;
	}

	template <typename T>
	void HashMap<T>::Insert(T& value)
	{
		_Insert(value);
	}

	template <typename T>
	void HashMap<T>::Insert(T&& value)
	{
		_Insert(value);
	}

	template <typename T>
	void HashMap<T>::Erase(T& value)
	{
		size_t index;
		const bool contains = Contains(value, index);
		assert(contains);

		const size_t length = _array.GetLength();
		assert(_count > 0);

		auto& keyPair = _array[index];

		// Check how big the key group is.
		size_t i = 1;
		while(i < length)
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
	bool HashMap<T>::Contains(T& value)
	{
		size_t n;
		return Contains(value, n);
	}

	template <typename T>
	size_t HashMap<T>::GetCount() const
	{
		return _count;
	}

	template <typename T>
	size_t HashMap<T>::GetHash(T& value)
	{
		assert(hasher);
		return hasher(value) % _array.GetLength();
	}

	template <typename T>
	bool HashMap<T>::Contains(T& value, size_t& outIndex)
	{
		const size_t length = _array.GetLength();
		assert(_count < length);

		// Get and use the hash as an index.
		const size_t hash = GetHash(value);

		for (size_t i = 0; i < length; ++i)
		{
			const size_t index = (hash + i) % length;
			auto& keyPair = _array[index];

			// If the hash is different, continue.
			if (keyPair.key != hash)
				continue;

			// If the actual value has been found.
			// We have to compare the values due to the fact that one hash might be generated more than once.
			if (keyPair.value == value)
			{
				outIndex = index;
				return true;
			}
		}

		return false;
	}

	template <typename T>
	void HashMap<T>::_Insert(T& value)
	{
		const size_t length = _array.GetLength();
		assert(_count < length);

		// If it already contains this value, replace the old one with the newer value.
		if (Contains(value))
			return;

		const size_t hash = GetHash(value);

		for (size_t i = 0; i < length; ++i)
		{
			const size_t index = (hash + i) % length;
			auto& keyPair = _array[index];
			// Set to true the first time the key group has been found.
			if (keyPair.key != SIZE_MAX)
				continue;

			keyPair.key = hash;
			keyPair.value = value;
			++_count;
			break;
		}
	}
}
