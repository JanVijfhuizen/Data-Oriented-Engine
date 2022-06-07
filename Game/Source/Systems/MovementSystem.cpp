#include "pch.h"
#include "Systems/MovementSystem.h"
#include "Systems/CollisionSystem.h"
#include "Utils/Math.h"
#include "JlbMath.h"

namespace game
{
	void MovementSystem::Update(const EngineOutData& outData, SystemChain& chain)
	{
		auto collisionSystem = chain.Get<CollisionSystem>();
		const float dt = outData.deltaTime;

		for (const auto& task : *this)
		{
			const bool isMoving = !math::IsZero(task.dir);
			const glm::vec2 dir = isMoving ? normalize(glm::vec2(task.dir)) : glm::vec2(0);

			const glm::vec2&& moveDelta = dir * task.moveSpeed * dt;
			const auto collisionTask = CollisionSystem::CreateDefaultTask(*task.collider, *task.transform, moveDelta);
			collisionSystem->Add(collisionTask);

			const float&& targetRotation = math::GetAngle(glm::vec2(0), task.dir);
			auto& rotation = task.transform->rotation;
			const float& rotationDelta = task.rotationSpeed * dt;
			rotation = isMoving ? math::SmoothAngle(rotation, targetRotation, rotationDelta) : rotation;
		}
		SetCount(0);
	}
}
