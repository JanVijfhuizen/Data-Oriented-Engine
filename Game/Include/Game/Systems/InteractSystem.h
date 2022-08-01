#pragma once
#include "VkEngine/Systems/TaskSystem.h"

namespace game
{
	struct Entity;

	struct InteractionTask final
	{
		size_t target = SIZE_MAX;
		size_t src = SIZE_MAX;

		void (*interaction)(Entity& target, Entity& src);
	};

	class InteractSystem final : public vke::TaskSystem<InteractionTask>
	{
	public:

	};
}
