#pragma once
#include "VkEngine/Components/Transform.h"
#include "VkEngine/Graphics/SubTexture.h"
#include "RenderSystem.h"
#include "VkEngine/Graphics/Camera.h"

namespace vke
{
#ifndef ENTITY_RENDER_SYSTEM_CAPACITY
#define ENTITY_RENDER_SYSTEM_CAPACITY 1024
#endif

	struct EntityRenderTask final
	{
		Transform transform{};
		SubTexture subTexture{};
	};

	class EntityRenderSystem final : public RenderSystem<EntityRenderTask, Camera>
	{
		[[nodiscard]] jlb::StringView GetTextureAtlasFilePath() const override;
		[[nodiscard]] jlb::StringView GetFragmentShaderPath() const override;
		[[nodiscard]] jlb::StringView GetVertexShaderPath() const override;
		[[nodiscard]] size_t DefineCapacity(const EngineData& info) override;
	};
}
