#pragma once
#include "VkEngine/Systems/TaskSystem.h"

namespace game
{
	struct PickupTask final
	{
		struct Hand final
		{
			glm::vec2 position{};
			float rotation = 0;
		} hands[2];

		glm::vec2 origin{};
		glm::vec2 itemOrigin{};
		size_t item = SIZE_MAX;
	};

	class PickupSystem : public vke::TaskSystem<PickupTask>
	{
	public:

	};
}

