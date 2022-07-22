#pragma once
#include "Bounds.h"
#include "Vector.h"

namespace jlb
{
	class StackAllocator;

	class BoundingVolumeHierarchy final
	{
	public:
		void Allocate(StackAllocator& allocator, uint32_t size);
		void Build(ArrayView<Bounds> instances, size_t nodeCapacity = 4);
		void Free(StackAllocator& allocator);

		[[nodiscard]] size_t GetIntersections(const Bounds& bounds, 
			ArrayView<Bounds> instances, ArrayView<uint32_t> outArray);

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

		[[nodiscard]] uint32_t QuickSort(const Bounds* instances, uint32_t from, uint32_t to, uint32_t nodeCapacity, uint32_t depth);
		void* GetIntersections(const Bounds& bounds, uint32_t current, 
			const ArrayView<Bounds>& instances, const ArrayView<uint32_t>& outArray, uint32_t& outIndex);
		[[nodiscard]] static bool Intersects(const Bounds& a, const Bounds& b);
	};
}
