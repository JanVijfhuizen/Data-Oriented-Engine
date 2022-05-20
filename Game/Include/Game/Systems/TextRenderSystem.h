#pragma once
#include "TaskSystem.h"
#include "StringView.h"
#include "RenderSystem.h"
#include "Components/Transform.h"

namespace game
{
#ifndef TEXT_RENDER_CHAR_CAPACITY
#define TEXT_RENDER_CHAR_CAPACITY 1e3
#endif

	struct TextRenderTask final
	{
		jlb::StringView text{};
		glm::vec2 leftTop;
		float spacingPct = .2f;
	};

	struct CharRenderTask final
	{
		Transform transform{};
		SubTexture subTexture{};
	};

	class TextRenderSystem final : public RenderSystem<CharRenderTask>
	{
	public:
		void AddAsCharRenderTasks(TextRenderTask& textRenderTask);

	private:
		[[nodiscard]] CreateInfo GetCreateInfo() override;

		void Allocate(const EngineOutData& outData, SystemChain& chain) override;

		using RenderSystem<CharRenderTask>::IncreaseRequestedLength;
	};
}
