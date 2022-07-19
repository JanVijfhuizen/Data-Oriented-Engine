#pragma once
#include <glm/geometric.hpp>
#include <glm/vec2.hpp>
#include "Array.h"
#include "JlbMath.h"
#include "StackAllocator.h"

namespace jlb
{
	class DistanceTree final
	{
	public:
		void Allocate(StackAllocator& allocator, size_t size);
		void Free(StackAllocator& allocator);
		[[nodiscard]] size_t Add(const glm::vec2& position);
		size_t GetInstancesInRange(const glm::vec2& position, float range, ArrayView<size_t> outArray);
		[[nodiscard]] size_t GetLength() const;

	private:
		struct Node final
		{
			size_t index = SIZE_MAX;
			glm::vec2 position{};
			float range = 0;
			size_t left = 0;
			size_t right = 0;
		};

		Array<Node> _nodes{};
		size_t _index = 0;

		[[nodiscard]] float CalculateDistance(const glm::vec2& position, const Node& node) const;
		void* GetInstancesInRange(const glm::vec2& position, float range, size_t current, size_t* outArray, size_t& arrayIndex);
	};

	inline void DistanceTree::Allocate(StackAllocator& allocator, const size_t size)
	{
		_nodes.Allocate(allocator, size);
	}

	inline void DistanceTree::Free(StackAllocator& allocator)
	{
		_nodes.Free(allocator);
	}


#include "iostream"


	inline size_t DistanceTree::Add(const glm::vec2& position)
	{
		assert(_index < _nodes.GetLength());
		
		size_t current = 0;
		bool stop = false;
		
		while(!stop)
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

			std::cout << current << " " << node.left << " " << node.right << std::endl;
			// Don't change the current node if stop is true.
			// Else, set the current node to the node closest by.
			current = stop ? current : leftDis < rightDis ? left : right;
		}
		std::cout << std::endl;
		return current;
	}

	inline size_t DistanceTree::GetInstancesInRange(const glm::vec2& position, const float range, ArrayView<size_t> outArray)
	{
		size_t index = 0;
		GetInstancesInRange(position, range, 0, outArray.data, index);
		return index;
	}

	inline size_t DistanceTree::GetLength() const
	{
		return _nodes.GetLength();
	}

	inline float DistanceTree::CalculateDistance(const glm::vec2& position, const Node& node) const
	{
		const bool empty = node.index == SIZE_MAX;
		return empty ? 0 : math::Max<float>(glm::distance(position, node.position) - node.range, 0);
	}

	inline void* DistanceTree::GetInstancesInRange(const glm::vec2& position, 
		const float range, const size_t current,
		size_t* outArray, size_t& arrayIndex)
	{
		const auto& node = _nodes[current];
		const bool isValid = node.index != SIZE_MAX;
		const bool stop = node.left == 0;
		// TODO actually do the distance calculation.
		const bool inRange = isValid ? glm::distance(position, node.position) - node.range - range < 0 : false;

		// Add the current node and increment the array index.
		outArray[arrayIndex] = node.index;
		arrayIndex += isValid;

		// Silly optimization to avoid branching.
		stop ? nullptr : GetInstancesInRange(position, range, node.left, outArray, arrayIndex);
		stop ? nullptr : GetInstancesInRange(position, range, node.right, outArray, arrayIndex);
		return nullptr;
	}
}
