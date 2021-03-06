#include "BVH.h"
#include "JlbMath.h"
#include "StackAllocator.h"
#include "glm/geometric.hpp"

namespace jlb
{
	void BoundingVolumeHierarchy::Allocate(StackAllocator& allocator, const uint32_t size)
	{
		assert(size > 0);
		_nodes.Allocate(allocator, size);
		_indexes.Allocate(allocator, size);
	}

	void BoundingVolumeHierarchy::Build(const ArrayView<Bounds> instances, const size_t nodeCapacity)
	{
		const uint32_t length = instances.length;
		assert(length > 0);
		_nodes.Fill();
		_nodes.SetCount(0);
		for (uint32_t i = 0; i < length; ++i)
			_indexes[i] = i;
		QuickSort(instances.data, 0, length, nodeCapacity, 0);
	}

	void BoundingVolumeHierarchy::Free(StackAllocator& allocator)
	{
		_indexes.Free(allocator);
		_nodes.Free(allocator);
	}

	bool BoundingVolumeHierarchy::Intersects(const Bounds& a, const Bounds& b)
	{
		return (a.layers & b.layers) == 0 ? false : a.lBot.x <= b.rTop.x && a.rTop.x >= b.lBot.x && a.lBot.y <= b.rTop.y && a.rTop.y >= b.lBot.y;
	}

	size_t BoundingVolumeHierarchy::GetIntersections(const Bounds& bounds, 
		const ArrayView<Bounds> instances, const ArrayView<uint32_t> outArray)
	{
		assert(outArray.length > 0);
		uint32_t outIndex = 0;
		GetIntersections(bounds, 0, instances, outArray, outIndex);
		return outIndex;
	}

	uint32_t BoundingVolumeHierarchy::QuickSort(const Bounds* instances, 
		const uint32_t from, const uint32_t to, 
		const uint32_t nodeCapacity, const uint32_t depth)
	{
		// Calculate median and bounds.
		glm::ivec2 median{};
		Bounds bounds{};
		auto& lBot = bounds.lBot;
		auto& rTop = bounds.rTop;

		for (uint32_t i = from; i < to; ++i)
		{
			const auto& instance = instances[_indexes[i]];
			const auto& lBotInstance = instance.lBot;
			const auto& rTopInstance = instance.rTop;

			median += instance.GetCenter();
			lBot.x = math::Min(lBot.x, lBotInstance.x);
			lBot.y = math::Min(lBot.y, lBotInstance.y);
			rTop.x = math::Max(rTop.x, rTopInstance.x);
			rTop.y = math::Max(rTop.y, rTopInstance.x);
			bounds.layers |= instance.layers;
		}

		median /= to - from;

		uint32_t partitionIndex = to;
		const bool partitionXAxis = depth % 2 == 0;

		for (uint32_t i = from; i < partitionIndex;)
		{
			const auto& instance = instances[_indexes[i]];
			const auto center = instance.GetCenter();
			const bool left = partitionXAxis ? center.x < median.x : center.y < median.y;

			const uint32_t index = left ? i : partitionIndex - 1;

			// Swap.
			const uint32_t temp = _indexes[index];
			_indexes[index] = _indexes[i];
			_indexes[i] = temp;

			partitionIndex -= !left;
			i += left;
		}

		const bool isLeaf = from + nodeCapacity >= to;
		const uint32_t index = _nodes.GetCount();
		auto& node = _nodes.Add();
		node.bounds = bounds;
		node.begin = isLeaf ? from : 0;
		node.end = isLeaf ? to : 0;
		node.children[0] = isLeaf ? 0 : QuickSort(instances, from, partitionIndex, nodeCapacity, depth + 1);
		node.children[1] = isLeaf ? 0 : QuickSort(instances, partitionIndex, to, nodeCapacity, depth + 1);

		return index;
	}

	void* BoundingVolumeHierarchy::GetIntersections(const Bounds& bounds, const uint32_t current,
		const ArrayView<Bounds>& instances, const ArrayView<uint32_t>& outArray, uint32_t& outIndex)
	{
		const auto& node = _nodes[current];

		const uint32_t aChild = node.children[0];
		const uint32_t bChild = node.children[1];

		bool maxIntersectionsReached = false;

		// Check objects in range.
		for (uint32_t i = node.begin; i < node.end; ++i)
		{
			const uint32_t index = _indexes[i];
			const auto& instance = instances[index];
			const bool intersects = Intersects(instance, bounds);
			outArray[outIndex] = index;
			outIndex += intersects;

			maxIntersectionsReached = outIndex == outArray.length;
			i = maxIntersectionsReached ? node.end : i;
		}

		if(!maxIntersectionsReached)
		{
			const auto& aChildNode = _nodes[aChild];
			const auto& bChildNode = _nodes[bChild];

			aChild == 0 ? nullptr : !Intersects(aChildNode.bounds, bounds) ? nullptr :
				GetIntersections(bounds, aChild, instances, outArray, outIndex);
			bChild == 0 ? nullptr : !Intersects(bChildNode.bounds, bounds) ? nullptr :
				GetIntersections(bounds, bChild, instances, outArray, outIndex);
		}
		
		return nullptr;
	}
}