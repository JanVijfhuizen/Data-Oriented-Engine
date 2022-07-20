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

		[[nodiscard]] size_t GetIntersections(const glm::vec2& position, const glm::vec2& scale, 
			ArrayView<Instance> instances, ArrayView<uint32_t> outArray);

	private:
		struct Node final
		{
			glm::vec2 lBot{};
			glm::vec2 rTop{};
			uint32_t children[2]{0, 0};
			uint32_t begin = 0;
			uint32_t end = 0;
		};

		Vector<Node> _nodes{};
		Array<uint32_t> _indexes{};

		[[nodiscard]] uint32_t QuickSort(const Instance* instances, uint32_t from, uint32_t to, uint32_t nodeCapacity, uint32_t depth);
		void* GetIntersections(const glm::vec2& position, const glm::vec2& scale, uint32_t current, 
			const ArrayView<Instance>& instances, const ArrayView<uint32_t>& outArray, uint32_t& outIndex);
		[[nodiscard]] bool IntersectsBounds(const glm::vec2& lBot, const glm::vec2& rTop, const glm::vec2& position, const glm::vec2& scale) const;
		[[nodiscard]] bool IntersectsObjects(const glm::vec2& aPos, const glm::vec2& aScale, const glm::vec2& bPos, const glm::vec2& bScale) const;
	};

	inline void BoundingVolumeHierarchy::Allocate(StackAllocator& allocator, const ArrayView<Instance> instances, const size_t nodeCapacity)
	{
		const auto& length = instances.length;
		assert(length > 0);
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

	inline size_t BoundingVolumeHierarchy::GetIntersections(
		const glm::vec2& position, const glm::vec2& scale,
		const ArrayView<Instance> instances, const ArrayView<uint32_t> outArray)
	{
		uint32_t outIndex = 0;
		GetIntersections(position, scale, 0, instances, outArray, outIndex);
		return outIndex;
	}

	inline uint32_t BoundingVolumeHierarchy::QuickSort(
		const Instance* instances, const uint32_t from, const uint32_t to, const uint32_t nodeCapacity, const uint32_t depth)
	{
		// Calculate median and bounds.
		glm::vec2 median{};
		glm::vec2 lBot{};
		glm::vec2 rTop{};

		for (uint32_t i = from; i < to; ++i) 
		{
			const auto& instance = instances[_indexes[i]];
			const auto& position = instance.position;
			const auto scale = instance.scale * .5f;

			median += position;
			lBot.x = math::Min(lBot.x, position.x - scale.x);
			lBot.y = math::Min(lBot.y, position.y - scale.y);
			rTop.x = math::Max(rTop.x, position.x + scale.x);
			rTop.y = math::Max(rTop.y, position.y + scale.x);
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

		const bool isLeaf = from + nodeCapacity >= to;
		const uint32_t index = _nodes.GetCount();
		auto& node = _nodes.Add();
		node.lBot = lBot;
		node.rTop = rTop;
		node.begin = isLeaf ? from : 0;
		node.end = isLeaf ? to : 0;
		node.children[0] = isLeaf ? 0 : QuickSort(instances, from, partitionIndex, nodeCapacity, depth + 1);
		node.children[1] = isLeaf ? 0 : QuickSort(instances, partitionIndex, to, nodeCapacity, depth + 1);

		return index;
	}

	inline void* BoundingVolumeHierarchy::GetIntersections(
		const glm::vec2& position, const glm::vec2& scale, const uint32_t current,
		const ArrayView<Instance>& instances, const ArrayView<uint32_t>& outArray, uint32_t& outIndex)
	{
		const auto& node = _nodes[current];

		const uint32_t aChild = node.children[0];
		const uint32_t bChild = node.children[1];

		// Check objects in range.
		for (uint32_t i = node.begin; i < node.end; ++i)
		{
			const uint32_t index = _indexes[i];
			const auto& instance = instances[index];
			const bool intersects = IntersectsObjects(instance.position, instance.scale, position, scale);
			outArray[outIndex] = index;
			outIndex += intersects;
		}

		const auto& aChildNode = _nodes[aChild];
		const auto& bChildNode = _nodes[bChild];

		aChild == 0 ? nullptr : !IntersectsBounds(aChildNode.lBot, aChildNode.rTop, position, scale) ? nullptr :
			GetIntersections(position, scale, aChild, instances, outArray, outIndex);
		bChild == 0 ? nullptr : !IntersectsBounds(bChildNode.lBot, bChildNode.rTop, position, scale) ? nullptr :
			GetIntersections(position, scale, bChild, instances, outArray, outIndex);
		return nullptr;
	}

	inline bool BoundingVolumeHierarchy::IntersectsBounds(
		const glm::vec2& lBot, const glm::vec2& rTop,
		const glm::vec2& position, const glm::vec2& scale) const
	{
		const glm::vec2 thresh = math::Threshold(position, lBot, rTop) * 2.f;
		return thresh.x < scale.x && thresh.y < scale.y;
	}

	inline bool BoundingVolumeHierarchy::IntersectsObjects(
		const glm::vec2& aPos, const glm::vec2& aScale,
		const glm::vec2& bPos, const glm::vec2& bScale) const
	{
		const glm::vec2 offset = aPos - bPos;
		return abs(offset.x) < (aScale.x + bScale.x) * .5f && abs(offset.y) < (aScale.y + bScale.y) * .5f;
	}
}
