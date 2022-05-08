#pragma once
#include "TaskSystem.h"
#include "Graphics/Skeleton.h"

namespace game
{
	struct Texture;

	struct SkeletonTask final
	{
		Skeleton* skeleton = nullptr;
	};

	class SkeletonSystem final : public jlb::TaskSystem<SkeletonTask>
	{
	public:
		[[nodiscard]] Skeleton GenerateSkeleton(jlb::StackAllocator& allocator, const Texture& texture, Skeleton::Type type) const;
	};
}
