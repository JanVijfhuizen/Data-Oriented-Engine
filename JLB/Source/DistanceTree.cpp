#include "DistanceTree.h"
#include "JlbMath.h"

namespace jlb
{
	void DistanceTree::Allocate(StackAllocator& allocator, const uint32_t size)
	{
		_nodes.Allocate(allocator, size);
	}

	void DistanceTree::Free(StackAllocator& allocator)
	{
		_nodes.Free(allocator);
	}

	uint32_t DistanceTree::Add(const Bounds& bounds)
	{
		return Add(bounds, bounds.GetCenter());
	}

	uint32_t DistanceTree::Add(const Bounds& bounds, const glm::ivec2& center)
	{
		assert(_index < _nodes.GetLength());

		uint32_t current = 0;
		bool stop = false;

		auto& lBot = bounds.lBot;
		auto& rTop = bounds.rTop;

		while (!stop)
		{
			auto& node = _nodes[current];
			auto& nodeBounds = node.bounds;
			auto& lBotNode = nodeBounds.lBot;
			auto& rTopNode = nodeBounds.rTop;

			// Check if the current node is empty.
			// If so, stop at this node.
			stop = node.index == UINT32_MAX;

			// Change the index + position + next if this node is a leaf.
			node.index = stop ? _index : node.index;
			lBotNode = stop ? lBot : lBotNode;
			rTopNode = stop ? rTop : rTopNode;

			auto& instance = node.instance;
			instance = stop ? bounds : instance;

			// Updates bounds to reflect new instance.
			lBotNode.x = math::Min(lBot.x, lBotNode.x);
			lBotNode.y = math::Min(lBot.y, lBotNode.y);
			rTopNode.x = math::Max(lBot.x, rTopNode.x);
			rTopNode.y = math::Max(lBot.y, rTopNode.y);
			instance.layers |= bounds.layers;

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
			const bool checkLeft = right != _index && left != 0;
			const bool checkRight = left != _index && right != 0;
			const float leftDis = checkLeft ? CalculateDistance(center, leftNode.bounds) : FLT_MAX;
			const float rightDis = checkRight ? CalculateDistance(center, rightNode.bounds) : FLT_MAX;

			// Don't change the current node if stop is true.
			// Else, set the current node to the node closest by.
			current = stop ? current : leftDis < rightDis ? left : right;
		}

		return current;
	}

	uint32_t DistanceTree::GetInstancesInRange(const Bounds& bounds, const ArrayView<uint32_t> outArray)
	{
		assert(outArray.length > 0);
		uint32_t index = 0;
		GetInstancesInRange(bounds, 0, outArray, index);
		return index;
	}

	uint32_t DistanceTree::GetLength() const
	{
		return _nodes.GetLength();
	}

	void DistanceTree::Clear()
	{
		for (size_t i = 0; i < _index; ++i)
			_nodes[i] = {};
		_index = 0;
	}

	float DistanceTree::CalculateDistance(const glm::ivec2& position, const Bounds& bounds) const
	{
		const auto& lBot = bounds.lBot;
		const auto& rTop = bounds.rTop;

		const bool xInBounds = position.x >= lBot.x && position.x <= rTop.x;
		const bool yInBounds = position.y >= lBot.y && position.y <= rTop.y;

		const size_t xOffset = xInBounds ? 0 : math::Min(abs(position.x - lBot.x), abs(position.x - rTop.x));
		const size_t yOffset = yInBounds ? 0 : math::Min(abs(position.y - lBot.y), abs(position.y - rTop.y));

		return glm::length(glm::vec2(xOffset, yOffset));
	}

	void* DistanceTree::GetInstancesInRange(const Bounds& bounds, const uint32_t current, 
		const ArrayView<uint32_t>& outArray, uint32_t& arrayIndex)
	{
		const auto& node = _nodes[current];
		bool isValid = node.index != UINT32_MAX;

		const bool nodeIntersects = isValid ? bounds.Intersects(node.bounds) : false;
		const bool instanceIntersects = nodeIntersects ? bounds.Intersects(node.instance) : false;

		// Add the current node and increment the array index.
		outArray[arrayIndex] = node.index;
		arrayIndex += instanceIntersects;

		isValid = isValid ? arrayIndex < outArray.length && nodeIntersects : false;
		// Silly optimization to avoid branching.
		!isValid || node.left == 0 ? nullptr : GetInstancesInRange(bounds, node.left, outArray, arrayIndex);
		!isValid || node.right == 0 ? nullptr : GetInstancesInRange(bounds, node.right, outArray, arrayIndex);
		
		return nullptr;
	}
}
