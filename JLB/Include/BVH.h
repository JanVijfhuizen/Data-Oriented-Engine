#pragma once
#include <glm/vec2.hpp>

#include "JlbMath.h"
#include "Vector.h"
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

		void Allocate(StackAllocator& allocator, ArrayView<Instance> instances, size_t nodeCapacity = 4);
		void Free(StackAllocator& allocator);

	private:
		struct Node final
		{
			glm::vec2 lTop{};
			glm::vec2 rBot{};
			uint32_t children[2]{0, 0};
			uint32_t begin = 0;
			uint32_t end = 0;
		};

		Vector<Node> _nodes{};
		Array<uint32_t> _indexes{};

		uint32_t QuickSort(const Instance* instances, uint32_t from, uint32_t to, uint32_t nodeCapacity, uint32_t depth);
	};

	inline void BoundingVolumeHierarchy::Allocate(StackAllocator& allocator, const ArrayView<Instance> instances, const size_t nodeCapacity)
	{
		const auto& length = instances.length;
		_nodes.Allocate(allocator, length);
		_indexes.Allocate(allocator, length);

		for (uint32_t i = 0; i < length; ++i)
			_indexes[i] = i;

		QuickSort(instances.data, 0, length, nodeCapacity, 0);
	}

	inline void BoundingVolumeHierarchy::Free(StackAllocator& allocator)
	{
		_indexes.Free(allocator);
		_nodes.Free(allocator);
	}

	inline uint32_t BoundingVolumeHierarchy::QuickSort(
		const Instance* instances, const uint32_t from, const uint32_t to, const uint32_t nodeCapacity, const uint32_t depth)
	{
		// Calculate median and bounds.
		glm::vec2 median{};
		glm::vec2 lTop{};
		glm::vec2 rBot{};

		for (uint32_t i = from; i < to; ++i) 
		{
			const auto& instance = instances[_indexes[i]];
			const auto& position = instance.position;
			const auto scale = instance.scale * .5f;

			median += position;
			lTop.x = math::Min(lTop.x, position.x - scale.x);
			lTop.y = math::Max(lTop.y, position.y + scale.y);
			rBot.x = math::Max(rBot.x, position.x + scale.x);
			rBot.y = math::Min(rBot.y, position.y - scale.x);
		}

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

		const uint32_t index = _nodes.GetCount();
		auto& node = _nodes.Add();
		node.lTop = lTop;
		node.rBot = rBot;
		node.begin = from;
		node.end = to;
		node.children[0] = from + nodeCapacity >= partitionIndex ? 0 : QuickSort(instances, from, partitionIndex, nodeCapacity, depth + 1);
		node.children[1] = partitionIndex + nodeCapacity >= to ? 0 : QuickSort(instances, partitionIndex, to, nodeCapacity, depth + 1);

		return index;
	}
}
