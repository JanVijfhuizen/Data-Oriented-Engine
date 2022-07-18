#pragma once
#include "RenderSystem.h"
#include "VkEngine/Components/Transform.h"
#include "VkEngine/Graphics/SubTexture.h"

namespace vke
{
#ifndef UI_RENDER_SYSTEM_CAPACITY
#define UI_RENDER_SYSTEM_CAPACITY 1024
#endif

	struct UIRenderTask final
	{
		Transform transform{};
		SubTexture subTexture{};
	};

	class UIRenderSystem final : public RenderSystem<UIRenderTask>
	{
	public:
		[[nodiscard]] static glm::vec2 ScreenToWorldPos(glm::vec2 pos, const Camera& camera);
		[[nodiscard]] static glm::vec2 WorldToScreenPos(glm::vec2 pos, const Camera& camera);

	private:
		[[nodiscard]] jlb::StringView GetTextureAtlasFilePath() const override;
		[[nodiscard]] jlb::StringView GetFragmentShaderPath() const override;
		[[nodiscard]] jlb::StringView GetVertexShaderPath() const override;
		[[nodiscard]] size_t DefineMinimalUsage(const EngineData& info) override;
	};
}
