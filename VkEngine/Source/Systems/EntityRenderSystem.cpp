﻿#include "VkEngine/pch.h"
#include "VkEngine/Systems/EntityRenderSystem.h"

#include "VkEngine/Graphics/CameraUtils.h"

namespace vke
{
	jlb::StringView EntityRenderSystem::GetTextureAtlasFilePath() const
	{
		return "Textures/atlas.png";
	}

	jlb::StringView EntityRenderSystem::GetFragmentShaderPath() const
	{
		return "Shaders/frag.spv";
	}

	jlb::StringView EntityRenderSystem::GetVertexShaderPath() const
	{
		return "Shaders/vert.spv";
	}

	size_t EntityRenderSystem::DefineCapacity(const EngineData& info)
	{
		return ENTITY_RENDER_SYSTEM_CAPACITY;
	}

	bool EntityRenderSystem::ValidateOnTryAdd(const EntityRenderJob& jobs)
	{
		const bool culls = Culls(camera.position, camera.pixelSize, jobs.transform.position, glm::vec2(jobs.transform.scale));
		return culls ? false : RenderSystem<EntityRenderJob, EntityCamera>::ValidateOnTryAdd(jobs);
	}
}
