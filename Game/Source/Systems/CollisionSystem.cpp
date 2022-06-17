#include "pch.h"
#include "Systems/CollisionSystem.h"

namespace game
{
	DynamicCollisionTask CollisionSystem::CreateDefaultTask(Collider& collider, Transform& transform, const glm::vec2 delta)
	{
		StaticCollisionTask&& subTask{};
		subTask.collider = collider;
		subTask.transform = transform;
		subTask.transform.position += delta;

		DynamicCollisionTask task{};
		task.subTask = subTask;
		task.outTransform = &transform;
		return task;
	}

	void CollisionSystem::IncreaseRequestedLength(const size_t size, const bool isDynamic)
	{
		if (isDynamic)
			IncreaseRequestedLength(size);
		else
			_requestedStaticSize += size;
	}

	void CollisionSystem::Allocate(const EngineData& EngineData, SystemChain& chain)
	{
		TaskSystem<DynamicCollisionTask>::Allocate(EngineData, chain);
		_statics.Allocate(*EngineData.allocator, _requestedStaticSize);
		_validChecks.Allocate(*EngineData.allocator, GetRequestedLength());
	}

	void CollisionSystem::Free(const EngineData& EngineData, SystemChain& chain)
	{
		_validChecks.Free(*EngineData.allocator);
		_statics.Free(*EngineData.allocator);
		TaskSystem<DynamicCollisionTask>::Free(EngineData, chain);
	}

	void CollisionSystem::AddStatic(StaticCollisionTask& task)
	{
		_statics.Add(task);
	}

	void CollisionSystem::ClearStatics()
	{
		_statics.SetCount(0);
	}

	void CollisionSystem::Update(const EngineData& EngineData, SystemChain& chain)
	{
		const size_t count = GetCount();

		// Check all dynamic collisions.
		for (size_t i = 0; i < count; ++i)
		{
			auto& a = this->operator[](i);
			auto& validCheck = _validChecks[i] = true;

			for (size_t j = i + 1; j < count; ++j)
			{
				auto& b = this->operator[](i);

				if(Collides(a, b))
				{
					validCheck = false;
					break;
				}
			}
		}

		const size_t staticCount = _statics.GetCount();

		// Check all dynamic vs static collisions.
		for (size_t i = 0; i < count; ++i)
		{
			auto& a = this->operator[](i);
			auto& validCheck = _validChecks[i] = true;

			for (size_t j = 0; j < staticCount; ++j)
			{
				auto& b = _statics[j];

				if (Collides(a, b))
				{
					validCheck = false;
					break;
				}
			}
		}

		// Update positions.
		for (size_t i = 0; i < count; ++i)
		{
			auto& a = this->operator[](i);
			auto& isValid = _validChecks[i];

			if (isValid)
				a.outTransform->position = a.subTask.transform.position;
		}

		SetCount(0);
	}

	bool CollisionSystem::IsOutOfRange(const Transform& aDynamic, const Transform& bStatic) const
	{
		const float dis = glm::distance(aDynamic.position, bStatic.position);
		const float radius = (glm::length(glm::vec2(aDynamic.scale)) + glm::length(glm::vec2(bStatic.scale))) * .5f;
		return dis > radius;
	}

	bool CollisionSystem::Collides(const DynamicCollisionTask& a, const DynamicCollisionTask& b) const
	{
		auto& aTransform = a.subTask.transform;
		auto& bTransform = b.subTask.transform;

		return CircleCollides(aTransform, bTransform);
	}

	bool CollisionSystem::Collides(const DynamicCollisionTask& a, const StaticCollisionTask& b) const
	{
		const auto& aTransform = a.subTask.transform;
		const auto& bTransform = b.transform;

		if (IsOutOfRange(aTransform, bTransform))
			return false;

		const auto& aPos = aTransform.position;
		const auto& bPos = bTransform.position;

		const auto& aScale = aTransform.scale;
		const auto& bScale = bTransform.scale;

		// AABB collision.
		const bool xDis = aPos.x < bPos.x + bScale;
		const bool xDis2 = aPos.x + aScale > bPos.x;

		const bool yDis = aPos.y < bPos.y + bScale;
		const bool yDis2 = aPos.y + aScale > bPos.y;

		return xDis && xDis2 && yDis && yDis2;
	}

	bool CollisionSystem::CircleCollides(const Transform& a, const Transform& b) const
	{
		const float dis = glm::distance(a.position, b.position);
		const float radius = (a.scale + b.scale) * .5f;

		return dis < radius;
	}
}
