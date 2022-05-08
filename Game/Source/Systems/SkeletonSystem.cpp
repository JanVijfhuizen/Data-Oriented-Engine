#include "pch.h"
#include "Systems/SkeletonSystem.h"
#include "Graphics/TextureHandler.h"

namespace game
{
	Skeleton SkeletonSystem::GenerateSkeleton(jlb::StackAllocator& allocator, 
		const Texture& texture, const Skeleton::Type type) const
	{
		Skeleton skeleton{};
		auto& bones = skeleton.bones;
		switch (type) 
		{ 
		case Skeleton::Type::human:
			bones.Allocate(allocator, 6);
			// Head.
			bones[0] = TextureHandler::GenerateSubTexture(texture, glm::ivec2(0, 0), glm::ivec2(15, 5));
			// Body.
			bones[1] = TextureHandler::GenerateSubTexture(texture, glm::ivec2(4, 6), glm::ivec2(11, 10));
			// L Arm.
			bones[2] = TextureHandler::GenerateSubTexture(texture, glm::ivec2(0, 6), glm::ivec2(3, 15));
			// R Arm.
			bones[3] = TextureHandler::GenerateSubTexture(texture, glm::ivec2(12, 6), glm::ivec2(15, 15));
			// L Leg.
			bones[4] = TextureHandler::GenerateSubTexture(texture, glm::ivec2(4, 11), glm::ivec2(7, 15));
			// R leg.
			bones[5] = TextureHandler::GenerateSubTexture(texture, glm::ivec2(8, 11), glm::ivec2(11, 15));
			break;
		default: 
			break; 
		}

		return skeleton;
	}
}
