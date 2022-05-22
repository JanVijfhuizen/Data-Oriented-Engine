#pragma once
#include "TaskSystem.h"

namespace game
{
	struct MovementTask final
	{
		glm::vec2 dir;
		float speed;
		struct Collider* collider;
		struct Transform* transform;
	};

	class MovementSystem final : public TaskSystem<MovementTask>
	{
	protected:
		void Update(const EngineOutData& outData, SystemChain& chain) override;
	};
}
