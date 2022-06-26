#pragma once
#include "TaskSystem.h"

namespace vke
{
	struct EntityRenderTask final
	{
		
	};

	class EntityRenderSystem final : public TaskSystem<EntityRenderTask>
	{
		void OnUpdate(const EngineData& info, jlb::Systems<EngineData> systems,
			const jlb::Vector<EntityRenderTask>& tasks) override;
	};
}
