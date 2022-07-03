#include "VkEngine/pch.h"
#include "VkEngine/Systems/UIRenderSystem.h"

namespace vke
{
	glm::vec2 UIRenderSystem::ScreenToWorldPos(const glm::vec2 pos, const Camera& camera)
	{
		return (pos / camera.pixelSize + camera.position) / static_cast<float>(PIXEL_SIZE_ENTITY);
	}

	glm::vec2 UIRenderSystem::WorldToScreenPos(const glm::vec2 pos, const Camera& camera)
	{
		return ((pos * static_cast<float>(PIXEL_SIZE_ENTITY)) - camera.position) * camera.pixelSize;
	}

	jlb::StringView UIRenderSystem::GetTextureAtlasFilePath() const
	{
		return "Textures/Atlas.png";
	}

	jlb::StringView UIRenderSystem::GetFragmentShaderPath() const
	{
		return "Shaders/frag-ui.spv";
	}

	jlb::StringView UIRenderSystem::GetVertexShaderPath() const
	{
		return "Shaders/vert-ui.spv";
	}

	size_t UIRenderSystem::DefineMinimalUsage(const EngineData& info)
	{
		return 1024;
	}
}
