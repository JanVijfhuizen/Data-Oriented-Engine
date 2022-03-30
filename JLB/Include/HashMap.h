#pragma once
#include "Array.h"
#include "KeyPair.h"

namespace jlb
{
	/// <summary>
	/// Data container that that prioritizes quick lookup speed.
	/// </summary>
	template <typename T>
	class HashMap : public Array<KeyPair<T>>
	{
	public:
		// Function used to get a hash value from a value.
		size_t(*hasher)(T& value);

		/// <summary>
		/// Inserts a value into the hashset. Does not store duplicates.
		/// </summary>
		/// <param name="value">Value to be inserted.</param>
		void Insert(T& value);
		/// <summary>
		/// Inserts a value into the hashset. Does not store duplicates.
		/// </summary>
		/// <param name="value">Value to be inserted.</param>
		void Insert(T&& value);
		/// <summary>
		/// Checks if the HashMap contains a certain value.
		/// </summary>
		/// <param name="value">Value to be checked.</param>
		/// <returns>If the HashMap contains the value.</returns>
		[[nodiscard]] bool Contains(T& value);
		/// <summary>
		/// Remove by value.
		/// </summary>
		/// <param name="value">Value to be removed.</param>
		void Erase(T& value);

		/// <summary>
		/// Gets the amount of values in the HashMap.
		/// </summary>
		[[nodiscard]] size_t GetCount() const;

	protected:
		[[nodiscard]] size_t GetHash(T& value);
		[[nodiscard]] bool Contains(T& value, size_t& outIndex);
		void _Insert(T& value);

		KeyPair<T>& operator[](size_t index) override;
		Iterator<KeyPair<T>> begin() override;
		Iterator<KeyPair<T>> end() override;

	private:
		size_t _count = 0;
	};

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

		const size_t length = Array<KeyPair<T>>::GetLength();
		assert(_count > 0);

		auto& keyPair = Array<KeyPair<T>>::operator[](index);

		// Check how big the key group is.
		size_t i = 1;
		while(i < length)
		{
			const size_t otherIndex = (index + i) % length;
			auto& otherKeyPair = Array<KeyPair<T>>::operator[](otherIndex);
			if (otherKeyPair.key != keyPair.key)
				break;
			++i;
		}

		// Setting the keypair value to the default value.
		keyPair = {};
		// Move the key group one place backwards by swapping the first and last index.
		Array<KeyPair<T>>::Swap(index, index + i - 1);
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
		return hasher(value) % Array<KeyPair<T>>::GetLength();
	}

	template <typename T>
	bool HashMap<T>::Contains(T& value, size_t& outIndex)
	{
		const size_t length = Array<KeyPair<T>>::GetLength();
		assert(_count < length);

		// Get and use the hash as an index.
		const size_t hash = GetHash(value);

		for (size_t i = 0; i < length; ++i)
		{
			const size_t index = (hash + i) % length;
			auto& keyPair = Array<KeyPair<T>>::operator[](index);

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
		const size_t length = Array<KeyPair<T>>::GetLength();
		assert(_count < length);

		// If it already contains this value, replace the old one with the newer value.
		if (Contains(value))
			return;

		const size_t hash = GetHash(value);

		for (size_t i = 0; i < length; ++i)
		{
			const size_t index = (hash + i) % length;
			auto& keyPair = Array<KeyPair<T>>::operator[](index);
			// Set to true the first time the key group has been found.
			if (keyPair.key != SIZE_MAX)
				continue;

			keyPair.key = hash;
			keyPair.value = value;
			++_count;
			break;
		}
	}

	template <typename T>
	KeyPair<T>& HashMap<T>::operator[](const size_t index)
	{
		return Array<KeyPair<T>>::operator[](index);
	}

	template <typename T>
	Iterator<KeyPair<T>> HashMap<T>::begin()
	{
		return Array<KeyPair<T>>::begin();
	}

	template <typename T>
	Iterator<KeyPair<T>> HashMap<T>::end()
	{
		return Array<KeyPair<T>>::end();
	}
}
