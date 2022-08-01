#pragma once
#include "Bounds.h"
#include "Vector.h"

namespace jlb
{
	class StackAllocator;

	class BoundingVolumeHierarchy final
	{
	public:
		struct Instance final
		{
			Bounds bounds{};
			size_t entityIndex = SIZE_MAX;
		};

		void Allocate(StackAllocator& allocator, uint32_t size);
		void Build(ArrayView<Instance> instances, size_t nodeCapacity = 4);
		void Free(StackAllocator& allocator);

		[[nodiscard]] size_t GetIntersections(const Bounds& bounds, 
			ArrayView<Instance> instances, ArrayView<uint32_t> outArray);

	private:
		struct Node final
		{
			Bounds bounds{};
			uint32_t children[2]{0, 0};
			uint32_t begin = 0;
			uint32_t end = 0;
		};

		Vector<Node> _nodes{};
		Array<uint32_t> _indexes{};

		uint32_t QuickSort(const Instance* instances, uint32_t from, uint32_t to, uint32_t nodeCapacity, uint32_t depth);
		void* GetIntersections(const Bounds& bounds, uint32_t current, 
			const ArrayView<Instance>& instances, const ArrayView<uint32_t>& outArray, uint32_t& outIndex);
	};
}
