#pragma once
#include "TaskSystem.h"
#include "Graphics/Skeleton.h"

namespace game
{
	struct SkeletonTask final
	{
		Skeleton* skeleton = nullptr;
	};

	class SkeletonSystem final : public jlb::TaskSystem<SkeletonTask>
	{
	public:
		[[nodiscard]] static Skeleton GenerateSkeleton(jlb::StackAllocator& allocator, const SubTexture& subTexture, Skeleton::Type type);
	};
}
