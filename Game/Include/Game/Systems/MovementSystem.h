#pragma once
#include "TaskSystem.h"

namespace game
{
	struct MovementTask final
	{
		glm::vec2 dir;
		float speed;
		struct Transform* transform;
	};

	class MovementSystem final : public jlb::TaskSystem<MovementTask>
	{
	public:
		void Update();
	};
}
