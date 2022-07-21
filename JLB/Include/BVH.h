#pragma once
#include <glm/vec2.hpp>
#include "Vector.h"

namespace jlb
{
	class StackAllocator;

	class BoundingVolumeHierarchy final
	{
	public:
		struct Instance final
		{
			glm::vec2 position;
			glm::vec2 scale;
		};

		void Allocate(StackAllocator& allocator, uint32_t size);
		void Build(ArrayView<Instance> instances, size_t nodeCapacity = 4);
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
}
