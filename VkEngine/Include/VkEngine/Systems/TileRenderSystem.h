#pragma once
#include "RenderSystem.h"
#include "VkEngine/Graphics/RenderConventions.h"

namespace vke
{
	struct TileRenderTask final
	{
		glm::vec2 position;
		glm::vec2 scale{ vke::PIXEL_SIZE_ENTITY };
		SubTexture subTexture{};
	};

	class TileRenderSystem final : public RenderSystem<TileRenderTask>
	{
		[[nodiscard]] jlb::StringView GetTextureAtlasFilePath() const override;
		[[nodiscard]] jlb::StringView GetFragmentShaderPath() const override;
		[[nodiscard]] jlb::StringView GetVertexShaderPath() const override;
		[[nodiscard]] size_t DefineCapacity(const EngineData& info) override;
	};
}
