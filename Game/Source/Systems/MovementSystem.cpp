#include "pch.h"
#include "Systems/MovementSystem.h"
#include "Systems/CollisionSystem.h"

namespace game
{
	void MovementSystem::Update(const EngineOutData& outData, SystemChain& chain)
	{
		auto collisionSystem = chain.Get<CollisionSystem>();

		const float dt = outData.deltaTime;
		for (auto& task : *this)
		{
			const auto collisionTask = CollisionSystem::CreateDefaultTask(*task.collider, *task.transform, task.dir * task.speed * dt);
			collisionSystem->Add(collisionTask);
		}
		SetCount(0);
	}
}
