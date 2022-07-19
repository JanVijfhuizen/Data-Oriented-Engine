#pragma once
#include "RenderSystem.h"

namespace vke
{
	struct TileRenderTask final
	{
		glm::vec2 position;
		glm::vec2 shape{1};
		SubTexture subTexture{};
	};

	class TileRenderSystem final : public RenderSystem<TileRenderTask>
	{
		[[nodiscard]] jlb::StringView GetTextureAtlasFilePath() const override;
		[[nodiscard]] jlb::StringView GetFragmentShaderPath() const override;
		[[nodiscard]] jlb::StringView GetVertexShaderPath() const override;
	};
}
