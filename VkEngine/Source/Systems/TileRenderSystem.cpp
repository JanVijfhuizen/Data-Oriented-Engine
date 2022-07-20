#include "VkEngine/pch.h"
#include "VkEngine/Systems/TileRenderSystem.h"
#include "VkEngine/Graphics/CameraUtils.h"

namespace vke
{
	jlb::StringView TileRenderSystem::GetTextureAtlasFilePath() const
	{
		return "Textures/atlas-tile.png";
	}

	jlb::StringView TileRenderSystem::GetFragmentShaderPath() const
	{
		return "Shaders/frag-tile.spv";
	}

	jlb::StringView TileRenderSystem::GetVertexShaderPath() const
	{
		return "Shaders/vert-tile.spv";
	}

	size_t TileRenderSystem::DefineCapacity(const EngineData& info)
	{
		return 1024;
	}

	bool TileRenderSystem::ValidateOnTryAdd(const TileRenderTask& task)
	{
		const bool culls = Culls(camera.position, camera.pixelSize, task.position, task.shape);
		return culls ? false : RenderSystem<TileRenderTask, TileCamera>::ValidateOnTryAdd(task);
	}
}
