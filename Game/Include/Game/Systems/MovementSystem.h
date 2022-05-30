#pragma once
#include "TaskSystem.h"
#include "Utils/Math.h"
#include "Graphics/RenderConventions.h"

namespace game
{
	struct MovementTask final
	{
		glm::vec2 dir;
		float moveSpeed = 0.005f * renderConventions::ENTITY_SIZE;
		float rotationSpeed = math::PI / 200;
		struct Collider* collider;
		struct Transform* transform;
	};

	class MovementSystem final : public TaskSystem<MovementTask>
	{
	protected:
		void Update(const EngineOutData& outData, SystemChain& chain) override;
	};
}
