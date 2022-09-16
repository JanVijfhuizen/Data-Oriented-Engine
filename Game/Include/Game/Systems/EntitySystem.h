#pragma once
#include <Entities/Entity.h>
#include "VkEngine/Systems/JobSystem.h"

namespace game
{
	class EntitySystem final : public vke::JobSystem<Entity*>
	{
	protected:
		void EndFrame(const vke::EngineData& info, jlb::Systems<vke::EngineData> systems) override;
		[[nodiscard]] bool AutoClearOnFrameEnd() override;
	};
}
