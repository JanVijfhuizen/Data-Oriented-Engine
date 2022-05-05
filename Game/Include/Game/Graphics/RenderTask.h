#pragma once
#include "SubTexture.h"
#include "Components/Transform.h"

namespace game
{
	struct RenderTask final
	{
		Transform transform{};
		SubTexture subTexture{};
	};
}
