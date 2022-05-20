#include "pch.h"
#include "Systems/CollisionSystem.h"

namespace game
{
	CollisionTask CollisionSystem::CreateDefaultTask(Collider& collider, Transform& transform)
	{
		CollisionTask task{};
		task.collider = collider;
		task.transform = transform;
		task.colliderSrc = &collider;
		task.transformSrc = &transform;
		return task;
	}

	void CollisionSystem::Update(const EngineOutData& outData, SystemChain& chain)
	{
		const size_t count = GetCount();
		for (size_t i = 0; i < count; ++i)
		{
			auto& a = this->operator[](i);

			for (size_t j = i + 1; j < count; ++j)
			{
				auto& b = this->operator[](i);
				
			}
		}

		// Update positions.
		for (auto& task : *this)
		{
			task.colliderSrc->delta = {};
			task.transformSrc->position = task.transform.position;
		}

		SetCount(0);
	}
}
