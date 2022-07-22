#pragma once
#include "RenderSystem.h"
#include "VkEngine/Graphics/SubTexture.h"

namespace vke
{
#ifndef UI_RENDER_SYSTEM_CAPACITY
#define UI_RENDER_SYSTEM_CAPACITY 1024
#endif

	struct UICamera final
	{
		glm::vec2 position{};
		// Size of a pixel, where the window width equals 1.
		float pixelSize = 0.008f;
	};

	struct UIRenderTask final
	{
		glm::vec2 position;
		glm::vec2 scale{ 1 };
		SubTexture subTexture{};
	};

	class UIRenderSystem final : public RenderSystem<UIRenderTask, UICamera>
	{
	public:
		[[nodiscard]] static glm::vec2 ScreenToWorldPos(glm::vec2 pos, const UICamera& camera, const glm::ivec2& resolution);
		[[nodiscard]] static glm::vec2 WorldToScreenPos(glm::vec2 pos, const UICamera& camera, const glm::ivec2& resolution);

	private:
		[[nodiscard]] jlb::StringView GetTextureAtlasFilePath() const override;
		[[nodiscard]] jlb::StringView GetFragmentShaderPath() const override;
		[[nodiscard]] jlb::StringView GetVertexShaderPath() const override;
		[[nodiscard]] size_t DefineCapacity(const EngineData& info) override;
	};
}
