#include "DistanceTree.h"
#include "JlbMath.h"

namespace jlb
{
	void DistanceTree::Allocate(StackAllocator& allocator, const size_t size)
	{
		_nodes.Allocate(allocator, size);
	}

	void DistanceTree::Free(StackAllocator& allocator)
	{
		_nodes.Free(allocator);
	}

	size_t DistanceTree::Add(const glm::vec2& position)
	{
		assert(_index < _nodes.GetLength());

		size_t current = 0;
		bool stop = false;

		while (!stop)
		{
			// Check if the current node is empty.
			// If so, stop at this node.
			auto& node = _nodes[current];
			stop = node.index == SIZE_MAX;

			// Change the index + position + next if this node is a leaf.
			node.index = stop ? _index : node.index;
			node.position = stop ? position : node.position;

			// Updates range to equal that of the furthest most leaf branch.
			node.range = stop ? node.range : math::Max(node.range, CalculateDistance(position, node));

			_index += stop;

			// Get child node indexes.
			auto& left = node.left;
			auto& right = node.right;

			// Set up child nodes if needed.
			left = stop ? left : left == 0 ? _index : left;
			right = stop ? right : right == 0 ? left == _index ? right : _index : right;

			const auto& leftNode = _nodes[left];
			const auto& rightNode = _nodes[right];

			// Calculate distances for the child nodes.
			const float leftDis = right == _index ? SIZE_MAX : CalculateDistance(position, leftNode);
			const float rightDis = left == _index ? SIZE_MAX : CalculateDistance(position, rightNode);

			// Don't change the current node if stop is true.
			// Else, set the current node to the node closest by.
			current = stop ? current : leftDis < rightDis ? left : right;
		}

		return current;
	}

	size_t DistanceTree::GetInstancesInRange(const glm::vec2& position, const float range, ArrayView<size_t> outArray)
	{
		size_t index = 0;
		GetInstancesInRange(position, range, 0, outArray.data, index);
		return index;
	}

	size_t DistanceTree::GetLength() const
	{
		return _nodes.GetLength();
	}

	float DistanceTree::CalculateDistance(const glm::vec2& position, const Node& node) const
	{
		const bool empty = node.index == SIZE_MAX;
		return empty ? 0 : math::Max<float>(glm::distance(position, node.position) - node.range, 0);
	}

	void* DistanceTree::GetInstancesInRange(const glm::vec2& position,
		const float range, const size_t current,
		size_t* outArray, size_t& arrayIndex)
	{
		const auto& node = _nodes[current];
		const bool isValid = node.index != SIZE_MAX;

		const float dis = glm::distance(position, node.position) - range;
		// If the node's instance is in range. If so, we can add it to the array.
		const bool instanceInRange = isValid ? dis < 0 : false;
		// If the node's radius is large enough, the children are still worth looking into.
		const bool nodeInRange = isValid ? dis - node.range < 0 : false;

		// Add the current node and increment the array index.
		outArray[arrayIndex] = node.index;
		arrayIndex += instanceInRange;

		// Silly optimization to avoid branching.
		!nodeInRange || node.left == 0 ? nullptr : GetInstancesInRange(position, range, node.left, outArray, arrayIndex);
		!nodeInRange || node.right == 0 ? nullptr : GetInstancesInRange(position, range, node.right, outArray, arrayIndex);
		return nullptr;
	}
}
