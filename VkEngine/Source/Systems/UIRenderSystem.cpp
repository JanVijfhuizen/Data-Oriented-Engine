#include "VkEngine/pch.h"
#include "VkEngine/Systems/UIRenderSystem.h"
#include "VkEngine/Graphics/RenderConventions.h"

namespace vke
{
	glm::vec2 UIRenderSystem::ScreenToWorldPos(glm::vec2 pos, const UICamera& camera, const glm::ivec2& resolution)
	{
		pos.x *= static_cast<float>(resolution.x) / resolution.y;
		return (pos / camera.pixelSize + camera.position) / static_cast<float>(vke::PIXEL_SIZE_ENTITY);
	}

	glm::vec2 UIRenderSystem::WorldToScreenPos(glm::vec2 pos, const UICamera& camera, const glm::ivec2& resolution)
	{
		pos.x *= static_cast<float>(resolution.y) / resolution.x;
		return (pos * static_cast<float>(vke::PIXEL_SIZE_ENTITY) - camera.position) * camera.pixelSize;
	}

	jlb::StringView UIRenderSystem::GetTextureAtlasFilePath() const
	{
		return "Textures/atlas-ui.png";
	}

	jlb::StringView UIRenderSystem::GetFragmentShaderPath() const
	{
		return "Shaders/frag-ui.spv";
	}

	jlb::StringView UIRenderSystem::GetVertexShaderPath() const
	{
		return "Shaders/vert-ui.spv";
	}

	size_t UIRenderSystem::DefineCapacity(const EngineData& info)
	{
		return UI_RENDER_SYSTEM_CAPACITY;
	}
}
