#pragma once
#include "StringView.h"
#include "VkEngine/Graphics/RenderConventions.h"
#include "VkEngine/Systems/JobSystem.h"

namespace game
{
	struct TextRenderJob final
	{
		// Text to render. Can include lowercase characters, spaces and numbers.
		jlb::StringView text{};
		// Origin position for the text.
		glm::vec2 origin{};
		// Scale multiplier, relative to pixel size.
		size_t scale = vke::PIXEL_SIZE_ENTITY;
		// Overrides the length of the string.
		size_t lengthOverride = SIZE_MAX;
		// Ignores origin value to place it after the text of target job.
		size_t appendIndex = SIZE_MAX;
		// Space between characters. Can be negative.
		int32_t padding = -8;
		// Center the text on the origin.
		bool center = false;
		// Color of the text.
		glm::vec4 color{1};
		// In characters.
		size_t maxWidth = SIZE_MAX;

		[[nodiscard]] size_t GetLineCount() const;
		// In characters.
		[[nodiscard]] size_t GetWidth() const;
	};

	/*
	Forwards created jobs to the UIRenderSystem as UIRenderjobs.
	*/
	class TextRenderHandler final : public vke::JobSystem<TextRenderJob>
	{
		void OnPreUpdate(const vke::EngineData& info, jlb::Systems<vke::EngineData> systems,
			const jlb::NestedVector<TextRenderJob>& jobs) override;
		[[nodiscard]] size_t DefineCapacity(const vke::EngineData& info) override;
	};
}
