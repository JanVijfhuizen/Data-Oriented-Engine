#pragma once
#include "Array.h"
#include "SubTexture.h"

namespace game
{
	class Animation final
	{
	public:
		struct Frame final
		{
			SubTexture subTexture{};
			uint8_t delay = 0;
		};

		jlb::Array<Frame> frames{};
	};
}
