#pragma once
#include "Components/Transform.h"
#include "Graphics/SubTexture.h"
#include "RenderSystem.h"

namespace game
{
	struct EntityRenderTask final
	{
		Transform transform{};
		SubTexture subTexture{};
	};

	class EntityRenderSystem final : public RenderSystem<EntityRenderTask>
	{
	protected:
		[[nodiscard]] CreateInfo GetCreateInfo() override;
	};
}
