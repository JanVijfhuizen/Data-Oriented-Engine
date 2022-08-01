#pragma once
#include "VkEngine/Systems/TaskSystem.h"

namespace game
{
	struct Entity;

	struct InteractionTask final
	{
		size_t target = SIZE_MAX;
		size_t src = SIZE_MAX;

		void (*interaction)(Entity& target, Entity& src, void* data);
		char data[16];
	};

	class InteractSystem final : public vke::TaskSystem<InteractionTask>
	{
	public:
		[[nodiscard]] size_t DefineCapacity(const vke::EngineData& info) override;
		void OnUpdate(const vke::EngineData& info, jlb::Systems<vke::EngineData> systems, 
			const jlb::NestedVector<InteractionTask>& tasks) override;
	};
}
