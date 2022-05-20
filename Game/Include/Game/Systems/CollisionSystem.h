#pragma once
#include "TaskSystem.h"
#include "Components/Collider.h"
#include "Components/Transform.h"

namespace game
{
	// Due to the repeated looping over the tasks I am copying the components instead of referencing them.
	struct CollisionTask final
	{
		Collider collider;
		Transform transform;
		Collider* colliderSrc;
		Transform* transformSrc;
	};

	class CollisionSystem final : public TaskSystem<CollisionTask>
	{
	private:
		[[nodiscard]] static CollisionTask CreateDefaultTask(Collider& collider, Transform& transform);
		void Update(const EngineOutData& outData, SystemChain& chain) override;
	};
}
