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

	/*
	Used to define interactions between entities.
	*/
	class InteractSystem final : public vke::TaskSystem<InteractionTask>
	{
	public:
		[[nodiscard]] size_t DefineCapacity(const vke::EngineData& info) override;
	private:
		void OnPreUpdate(const vke::EngineData& info, jlb::Systems<vke::EngineData> systems,
			const jlb::NestedVector<InteractionTask>& tasks) override;
		[[nodiscard]] bool AutoClearOnFrameEnd() override;
	};
}
