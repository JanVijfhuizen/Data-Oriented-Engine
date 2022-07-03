#pragma once
#include "TaskSystem.h"
#include "VkEngine/Components/Transform.h"
#include "VkEngine/Graphics/Camera.h"
#include "VkEngine/Graphics/Shader.h"
#include "VkEngine/Graphics/Mesh.h"
#include "VkEngine/Graphics/Texture.h"
#include "VkEngine/Graphics/SubTexture.h"
#include "RenderSystem.h"

namespace vke
{
	struct EntityRenderTask final
	{
		Transform transform{};
		SubTexture subTexture{};
	};

	class EntityRenderSystem final : public RenderSystem<EntityRenderTask>
	{
		
	};
}
