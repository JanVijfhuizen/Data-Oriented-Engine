﻿#pragma once
#include "VkEngine/Systems/TaskSystem.h"
#include <Entities/Entity.h>

namespace game
{
	class EntitySystem final : public vke::TaskSystem<Entity>
	{
	public:
		[[nodiscard]] size_t DefineCapacity(const vke::EngineData& info) override;
	};
}
