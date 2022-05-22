#pragma once
#include "TaskSystem.h"
#include "StringView.h"
#include "RenderSystem.h"
#include "Components/Transform.h"

namespace game
{
#ifndef TEXT_RENDER_CHAR_CAPACITY
// Defines the maximum amount of characters that can be rendered by the TextRenderSystem.
#define TEXT_RENDER_CHAR_CAPACITY 1e3
#endif

	// Task that can be converted into multiple CharRenderTasks.
	struct TextRenderTask final
	{
		jlb::StringView text{};
		glm::vec2 leftTop;
		float spacingPct = .2f;
	};

	// Task to render a single character.
	struct CharRenderTask final
	{
		Transform transform{};
		SubTexture subTexture{};
	};

	/// <summary>
	/// System that renders UI text to the screen.
	/// </summary>
	class TextRenderSystem final : public RenderSystem<CharRenderTask>
	{
	public:
		// Convert a text render task into multiple char render tasks and add it to the system.
		void AddAsCharRenderTasks(TextRenderTask& textRenderTask);

	private:
		[[nodiscard]] CreateInfo GetCreateInfo() override;

		void Allocate(const EngineOutData& outData, SystemChain& chain) override;

		using RenderSystem<CharRenderTask>::IncreaseRequestedLength;
	};
}
