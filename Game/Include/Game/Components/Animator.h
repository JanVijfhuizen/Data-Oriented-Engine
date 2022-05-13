#pragma once

namespace game
{
	class Animation;

	struct Animator final
	{
		Animation* animation = nullptr;
		uint32_t index;
		uint32_t rDelay = 0;
		bool loop = true;
	};
}