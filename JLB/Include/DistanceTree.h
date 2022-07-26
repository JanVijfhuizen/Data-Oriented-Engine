#pragma once
#include <glm/geometric.hpp>
#include <glm/vec2.hpp>
#include "Array.h"
#include "Bounds.h"
#include "StackAllocator.h"

namespace jlb
{
	class DistanceTree final
	{
	public:
		void Allocate(StackAllocator& allocator, uint32_t size);
		void Free(StackAllocator& allocator);
		[[nodiscard]] uint32_t Add(const Bounds& bounds);
		uint32_t GetInstancesInRange(const Bounds& bounds, ArrayView<uint32_t> outArray);
		[[nodiscard]] uint32_t GetLength() const;
		void Clear();

	private:
		struct Node final
		{
			uint32_t index = UINT32_MAX;
			Bounds instance{};
			Bounds bounds{};
			uint32_t left = 0;
			uint32_t right = 0;
		};

		Array<Node> _nodes{};
		uint32_t _index = 0;

		[[nodiscard]] uint32_t Add(const Bounds& bounds, const glm::ivec2& center);
		[[nodiscard]] float CalculateDistance(const glm::ivec2& position, const Bounds& bounds) const;
		void* GetInstancesInRange(const Bounds& bounds, uint32_t current,
			const ArrayView<uint32_t>& outArray, uint32_t& arrayIndex);
	};
}
