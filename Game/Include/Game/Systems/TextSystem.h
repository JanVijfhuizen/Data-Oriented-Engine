#pragma once
#include "TaskSystem.h"
#include "StringView.h"
#include "RenderSystem.h"
#include "Components/Transform.h"

namespace game
{
	struct TextTask final
	{
		jlb::StringView text{};
		glm::vec2 leftTop;
		float spacingPct = .2f;
	};

	class TextSystem final : public TaskSystem<TextTask>
	{
	private:
		struct InternalRenderTask final
		{
			Transform transform{};
			SubTexture subTexture{};
		};

		const size_t _renderSizeUsage = 1000;
		RenderSystem<InternalRenderTask> _renderSystem{};

		void Update(const EngineOutData& outData, SystemChain& chain) override;
		void Allocate(const EngineOutData& outData, SystemChain& chain) override;
		void Free(const EngineOutData& outData, SystemChain& chain) override;
	};
}
