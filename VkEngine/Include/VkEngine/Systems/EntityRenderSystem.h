#pragma once
#include <glm/vec3.hpp>

#include "VkEngine/Components/Transform.h"
#include "VkEngine/Graphics/SubTexture.h"
#include "RenderSystem.h"
#include "VkEngine/Graphics/RenderConventions.h"

namespace vke
{
#ifndef ENTITY_RENDER_SYSTEM_CAPACITY
#define ENTITY_RENDER_SYSTEM_CAPACITY 1024
#endif

	struct EntityCamera final
	{
		glm::vec2 position{};
		glm::vec3 lightDir{0, 0, 1};
		// Size of a pixel, where the window width equals 1.
		float pixelSize = 0.008f;
	private:
		int32_t _tileSize = PIXEL_SIZE_ENTITY;
	};

	struct EntityRenderJob final
	{
		Transform transform{};
		SubTexture subTexture{};
	};

	class EntityRenderSystem final : public RenderSystem<EntityRenderJob, EntityCamera>
	{
		[[nodiscard]] jlb::StringView GetTextureAtlasFilePath() const override;
		[[nodiscard]] jlb::StringView GetFragmentShaderPath() const override;
		[[nodiscard]] jlb::StringView GetVertexShaderPath() const override;
		[[nodiscard]] size_t DefineCapacity(const EngineData& info) override;
		[[nodiscard]] bool ValidateOnTryAdd(const EntityRenderJob& jobs) override;
	};
}
