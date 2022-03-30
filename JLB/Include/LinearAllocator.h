#pragma once

namespace jlb
{
	/// <summary>
	/// A class that allocates a big chunk of memory, where the memory can then be (re)used for smaller allocations.<br>
	/// It is only capable of allocating as a stack, so every new allocation will be on top of the last one.<br>
	/// This also means that you can only free the newest allocation at any given time.<br>
	/// Unlike a free list allocator, this does not result in memory fragmentation.
	/// </summary>
	class LinearAllocator final
	{
	public:
		explicit LinearAllocator(size_t size);
		~LinearAllocator();

		LinearAllocator(LinearAllocator& other) = delete;
		LinearAllocator(LinearAllocator&& other) = delete;
		LinearAllocator& operator=(LinearAllocator& other) = delete;
		LinearAllocator& operator=(LinearAllocator&& other) = delete;

		/// <summary>
		/// Allocates a chunk of memory.
		/// </summary>
		/// <param name="size">The size of the to be allocated memory.<br> 
		/// Take note that the allocation takes up an extra sizeof(size_t) amount of space.</param>
		/// <returns>Pointer to the allocated memory.</returns>
		[[nodiscard]] void* Malloc(size_t size);
		/// <summary>
		/// Frees the last allocation.<br>
		/// Does not call destructors.
		/// </summary>
		void Free();

		/// <summary>
		/// Wrapper method for Malloc. Immediately casts the allocated memory to one or multiple classes of type T.<br>
		/// Does not call constructors.
		/// </summary>
		/// <typeparam name="T">Type of classes to be allocated.</typeparam>
		/// <param name="count">Amount of classes to be allocated.</param>
		/// <returns>Pointer to the allocated memory.</returns>
		template <typename T>
		[[nodiscard]] T* New(size_t count = 1);

		/// <summary>
		/// Returns the amount of free memory remaining.
		/// </summary>
		[[nodiscard]] size_t GetAvailableMemorySpace() const;

		/// <summary>
		/// Returns whether or not the allocator is in use.
		/// </summary>
		[[nodiscard]] bool IsEmpty() const;

	private:
		// Pointer to the big chunk of memory, from which everything is allocated.
		size_t* _memory = nullptr;
		// The total size of the big chunk of memory.
		size_t _size = 0;
		// The current memory index where new allocations will take place.
		size_t _current = 0;

		/// <summary>
		/// Converts device size into chunk size.<br>
		/// Because the big chunk of memory is allocated as an array of size_t's, 
		/// this is used to convert the device size into a proper index.
		/// </summary>
		/// <param name="size"></param>
		/// <returns></returns>
		[[nodiscard]] static size_t ToChunkSize(size_t size);
	};

	template <typename T>
	T* LinearAllocator::New(const size_t count)
	{
		return reinterpret_cast<T*>(Malloc(sizeof(T) * count));
	}
}