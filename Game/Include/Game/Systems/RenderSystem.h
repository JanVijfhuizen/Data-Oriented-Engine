#pragma once
#include "EntitySystem.h"

namespace game
{
	struct Light;
	struct Transform;
	struct Renderer;

	struct RenderTask final
	{
		
	};

	class RenderSystem final : public jlb::EntitySystem<RenderTask>
	{
	public:
		void Update(vk::App& app);

		[[nodiscard]] static RenderTask CreateDefaultTask(Renderer& renderer, Transform& transform);
	};
}
