#pragma once
#include "VkEngine/Systems/TaskSystem.h"
#include <Entities/Entity.h>

namespace game
{
	
	/*
	Entities can be added here to be interacted with.
	The interaction system will use this entity system to update their variables based on interactions.
	*/
	class EntitySystem final : public vke::TaskSystem<EntityData>
	{
	public:
		[[nodiscard]] size_t DefineCapacity(const vke::EngineData& info) override;
	private:
		void OnPreUpdate(const vke::EngineData& info, jlb::Systems<vke::EngineData> systems,
			const jlb::NestedVector<EntityData>& tasks) override;
		[[nodiscard]] bool AutoClearOnFrameEnd() override;
	};
}
