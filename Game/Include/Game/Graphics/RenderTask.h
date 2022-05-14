#pragma once
#include "SubTexture.h"
#include "Components/Transform.h"
#include "Systems/RenderSystem.h"

namespace game
{
	struct RenderTask final
	{
		Transform transform{};
		SubTexture subTexture{};
	};

	typedef RenderSystem<RenderTask> EntityRenderSystem;
}
