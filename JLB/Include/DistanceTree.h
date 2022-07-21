#pragma once
#include <glm/geometric.hpp>
#include <glm/vec2.hpp>
#include "Array.h"
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
		void Clear();

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
		void* GetInstancesInRange(const glm::vec2& position, float range, size_t current, 
			const ArrayView<size_t>& outArray, size_t& arrayIndex);
	};
}
