#pragma once
#include "TaskSystem.h"
#include "Components/Transform.h"
#include "Components/Collider.h"

namespace game
{
	struct Collider;

	struct StaticCollisionTask final
	{
		Collider collider;
		Transform transform;
	};

	struct DynamicCollisionTask final
	{
		StaticCollisionTask subTask;
		Transform* outTransform;
	};

	class CollisionSystem final : public TaskSystem<DynamicCollisionTask>
	{
	public:
		[[nodiscard]] static DynamicCollisionTask CreateDefaultTask(Collider& collider, Transform& transform, glm::vec2 delta);

		void IncreaseRequestedLength(size_t size, bool isDynamic);
		void Allocate(const EngineOutData& outData, SystemChain& chain) override;
		void Free(const EngineOutData& outData, SystemChain& chain) override;

		void AddStatic(StaticCollisionTask& task);
		void ClearStatics();

	private:
		size_t _requestedStaticSize = 0;
		Vector<StaticCollisionTask> _statics{};
		Array<bool> _validChecks{};

		void Update(const EngineOutData& outData, SystemChain& chain) override;

		using TaskSystem<DynamicCollisionTask>::IncreaseRequestedLength;

		bool Collides(DynamicCollisionTask& a, DynamicCollisionTask& b);
		bool Collides(DynamicCollisionTask& a, StaticCollisionTask& b);
	};
}
