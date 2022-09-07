#pragma once
#include "StackAllocator.h"
#include "Array.h"

namespace vk
{
	struct MemBlock;
	struct App;

	class StackAllocator final
	{
	public:
		void Allocate(const App& app, size_t pageSize = 4096);
		void Free(const App& app);

		[[nodiscard]] MemBlock AllocateBlock(const App& app, VkDeviceSize size, VkDeviceSize alignment, uint32_t poolId);
		void FreeBlock(const MemBlock& memBlock) const;
		[[nodiscard]] uint32_t GetPoolId(const App& app, uint32_t typeFilter, VkMemoryPropertyFlags properties) const;
		[[nodiscard]] bool IsEmpty() const;

	private:
		using CpuAllocator = jlb::StackAllocator;

		struct Pool final
		{
			VkDeviceMemory memory = VK_NULL_HANDLE;
			VkMemoryPropertyFlags memPropertyFlags;

			VkDeviceSize size = 0;
			VkDeviceSize remaining = 0;
			VkDeviceSize alignment;
			size_t allocId = 0;
			Pool* next = nullptr;
		};

		size_t _pageSize;
		CpuAllocator _cpuAllocator{};
		jlb::Array<Pool> _rootPools{};

		void InitPool(Pool& pool, const App& app, VkDeviceSize size, VkDeviceSize alignment, uint32_t poolId) const;
		[[nodiscard]] static VkDeviceSize CalculateBufferSize(VkDeviceSize size, VkDeviceSize alignment);
	};
}
