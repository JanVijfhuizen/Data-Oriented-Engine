﻿#pragma once
#include "RenderSystem.h"
#include "VkEngine/Graphics/RenderConventions.h"

namespace vke
{
	struct TileCamera final
	{
		glm::vec2 position{};
		// Size of a pixel, where the window width equals 1.
		float pixelSize = 0.008f;
		int32_t tileSize = PIXEL_SIZE_ENTITY;
	};

	struct TileRenderTask final
	{
		glm::vec2 position;
		glm::vec2 shape{ 1 };
		SubTexture subTexture{};
	};

	class TileRenderSystem final : public RenderSystem<TileRenderTask, TileCamera>
	{
		[[nodiscard]] jlb::StringView GetTextureAtlasFilePath() const override;
		[[nodiscard]] jlb::StringView GetFragmentShaderPath() const override;
		[[nodiscard]] jlb::StringView GetVertexShaderPath() const override;
		[[nodiscard]] size_t DefineCapacity(const EngineData& info) override;
		bool ValidateOnTryAdd(const TileRenderTask& task) override;
	};
}
