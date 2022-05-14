#include "pch.h"
#include "Systems/MovementSystem.h"
#include "Components/Transform.h"

namespace game
{
	void MovementSystem::Update(const EngineOutData& outData, SystemChain& chain)
	{
		for (auto& task : *this)
			task.transform->position += task.dir * task.speed;
		SetCount(0);
	}
}
