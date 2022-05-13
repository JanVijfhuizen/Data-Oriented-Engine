#pragma once
#include "TaskSystem.h"
#include "StringView.h"
#include "RenderSystem.h"
#include "Components/Transform.h"

namespace game
{
	struct UITask final
	{
		jlb::StringView text{};
		glm::vec2 leftTop;
		float spacingPct = .2f;
	};

	class UISystem final : public jlb::TaskSystem<UITask>
	{
	public:
		void Allocate(const EngineOutData& engineOutData);
		void Free(const EngineOutData& engineOutData);
		void Update(const EngineOutData& engineOutData);

	private:
		using TaskSystem<UITask>::Allocate;

		struct InternalRenderTask
		{
			Transform transform{};
			SubTexture subTexture{};
		};

		const size_t _renderSizeUsage = 1000;
		RenderSystem<InternalRenderTask> _renderSystem{};
	};
}
