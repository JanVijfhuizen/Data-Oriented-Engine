#pragma once

namespace jlb
{
	class StackAllocator final
	{
	public:
		struct CreateInfo final
		{
			size_t pageSize = 4096;
			void* data = nullptr;
		};

		template <typename T>
		struct Allocation final
		{
			T* ptr;
			size_t id;
			size_t* src;
		};

		void Allocate(const CreateInfo& info = {});
		void Free();

		[[nodiscard]] Allocation<void> Malloc(size_t size);
		template <typename T>
		void MFree(const Allocation<T>& allocation);

		template <typename T>
		[[nodiscard]] Allocation<T> New(size_t count = 1);

	private:
		StackAllocator* _next = nullptr;
		size_t* _data = nullptr;
		size_t _size;
		size_t _current = 0;
		size_t _id = 0;

		[[nodiscard]] static size_t ToChunkSize(size_t size);
	};

	template <typename T>
	void StackAllocator::MFree(const Allocation<T>& allocation)
	{
		if (_data != allocation.src)
			return _next->MFree(allocation);
		assert(_id == allocation.id + 1);

		const size_t size = _data[_current - 1];
		_current -= size;
		_id--;
	}

	template <typename T>
	StackAllocator::Allocation<T> StackAllocator::New(const size_t count)
	{
		auto alloc = Malloc(sizeof(T) * count);
		Allocation<T> allocation{};
		allocation.id = alloc.id;
		allocation.ptr = reinterpret_cast<T*>(alloc.ptr);
		allocation.src = alloc.src;
		return allocation;
	}
}