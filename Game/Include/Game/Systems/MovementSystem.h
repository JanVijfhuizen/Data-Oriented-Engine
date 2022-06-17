#pragma once
#include "TaskSystem.h"
#include "Graphics/RenderConventions.h"
#include "JlbMath.h"

namespace game
{
	struct MovementTask final
	{
		glm::ivec2 dir{};
		float moveSpeed = 0.005f * renderConventions::ENTITY_SIZE;
		float rotationSpeed = jlb::math::PI / 200;

		struct Collider* collider = nullptr;
		struct Transform* transform = nullptr;
	};

	class MovementSystem final : public TaskSystem<MovementTask>
	{
	protected:
		void Update(const EngineData& EngineData, SystemChain& chain) override;
	};
}
