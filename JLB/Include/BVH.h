#pragma once
#include <glm/vec2.hpp>
#include "Array.h"
#include "StackAllocator.h"

namespace jlb
{
	class BoundingVolumeHierarchy final
	{
	public:
		struct Instance final
		{
			glm::vec2 position;
			glm::vec2 scale;
		};

		void Allocate(StackAllocator& allocator, ArrayView<Instance> instances);
		void Free(StackAllocator& allocator);

	private:
		struct Node final
		{
			glm::vec2 rTop{};
			glm::vec2 lBot{};
			uint32_t children[2]{0, 0};
			uint32_t begin = 0;
			uint32_t end = 0;
		};

		Array<Node> _nodes{};
		Array<uint32_t> _indexes{};

		void* QuickSort(const Instance* instances, uint32_t from, uint32_t to, uint32_t depth);
	};

	inline void BoundingVolumeHierarchy::Allocate(StackAllocator& allocator, const ArrayView<Instance> instances)
	{
		const auto& length = instances.length;
		_nodes.Allocate(allocator, length);
		_indexes.Allocate(allocator, length);

		for (uint32_t i = 0; i < length; ++i)
			_indexes[i] = i;

		QuickSort(instances.data, 0, length, 0);
	}

	inline void BoundingVolumeHierarchy::Free(StackAllocator& allocator)
	{
		_indexes.Free(allocator);
		_nodes.Free(allocator);
	}

	inline void* BoundingVolumeHierarchy::QuickSort(
		const Instance* instances, const uint32_t from, const uint32_t to, const uint32_t depth)
	{
		// Calculate median.
		glm::vec2 median{};
		for (uint32_t i = from; i < to; ++i)
			median += instances[_indexes[i]].position;
		median /= to - from;

		uint32_t partitionIndex = to;

		const bool partitionXAxis = depth % 2 == 0;

		for (uint32_t i = from; i < partitionIndex;)
		{
			const auto& instance = instances[_indexes[i]];
			const bool left = partitionXAxis ? instance.position.x < median.x : instance.position.y < median.y;

			const uint32_t index = left ? i : partitionIndex - 1;

			// Swap.
			const uint32_t temp = _indexes[index];
			_indexes[index] = _indexes[i];
			_indexes[i] = temp;

			partitionIndex -= !left;
			i += left;
		}

		from + 1 >= partitionIndex ? nullptr : QuickSort(instances, from, partitionIndex, depth + 1);
		partitionIndex + 1 >= to ? nullptr : QuickSort(instances, partitionIndex, to, depth + 1);
		return nullptr;
	}
}
