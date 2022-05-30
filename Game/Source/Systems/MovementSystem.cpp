#include "pch.h"
#include "Systems/MovementSystem.h"
#include "Systems/CollisionSystem.h"
#include "Utils/Math.h"

namespace game
{
	void MovementSystem::Update(const EngineOutData& outData, SystemChain& chain)
	{
		auto collisionSystem = chain.Get<CollisionSystem>();
		const float dt = outData.deltaTime;

		for (auto& task : *this)
		{
			const glm::vec2&& moveDelta = task.dir * task.moveSpeed * dt;
			const auto collisionTask = CollisionSystem::CreateDefaultTask(*task.collider, *task.transform, moveDelta);
			collisionSystem->Add(collisionTask);

			const float&& targetRotation = math::GetAngle(glm::vec2(0), task.dir);
			auto& rotation = task.transform->rotation;
			rotation = math::SmoothAngle(rotation, targetRotation, task.rotationSpeed * dt);
		}
		SetCount(0);
	}
}
