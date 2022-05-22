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

	void CollisionSystem::Allocate(const EngineOutData& outData, SystemChain& chain)
	{
		TaskSystem<DynamicCollisionTask>::Allocate(outData, chain);
		_statics.Allocate(*outData.allocator, _requestedStaticSize);
		_validChecks.Allocate(*outData.allocator, _requestedStaticSize);
	}

	void CollisionSystem::Free(const EngineOutData& outData, SystemChain& chain)
	{
		_validChecks.Free(*outData.allocator);
		_statics.Free(*outData.allocator);
		TaskSystem<DynamicCollisionTask>::Free(outData, chain);
	}

	void CollisionSystem::AddStatic(StaticCollisionTask& task)
	{
		_statics.Add(task);
	}

	void CollisionSystem::ClearStatics()
	{
		_statics.SetCount(0);
	}

	void CollisionSystem::Update(const EngineOutData& outData, SystemChain& chain)
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

	bool CollisionSystem::Collides(DynamicCollisionTask& a, DynamicCollisionTask& b)
	{
		return false;
	}

	bool CollisionSystem::Collides(DynamicCollisionTask& a, StaticCollisionTask& b)
	{
		return false;
	}
}
