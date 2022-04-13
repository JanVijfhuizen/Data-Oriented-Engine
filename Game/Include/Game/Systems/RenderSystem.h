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
		void Allocate(const EngineOutData& engineOutData, size_t size);
		void Free(jlb::LinearAllocator& allocator) override;

		void Update(vk::App& app);

		[[nodiscard]] static RenderTask CreateDefaultTask(Renderer& renderer, Transform& transform);

	private:
		using EntitySystem<RenderTask>::Allocate;
		using EntitySystem<RenderTask>::AllocateAndCopy;

		void LoadShader(const EngineOutData& engineOutData);
	};
}
