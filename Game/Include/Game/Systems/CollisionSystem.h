#pragma once
#include "TaskSystem.h"

namespace game
{
	struct Collider;
	struct Transform;

	struct CollisionTask final
	{
		Collider* collider;
		Transform* transform;
	};

	class CollisionSystem final : public TaskSystem<CollisionTask>
	{
	protected:
		void Update(const EngineOutData& outData, SystemChain& chain) override;
	};
}
