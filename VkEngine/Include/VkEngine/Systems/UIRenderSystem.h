#pragma once
#include "RenderSystem.h"
#include "VkEngine/Components/Transform.h"
#include "VkEngine/Graphics/SubTexture.h"

namespace game
{
	struct UIRenderTask final
	{
		vke::Transform transform{};
		vke::SubTexture subTexture{};
	};

	class UIRenderSystem final : public vke::RenderSystem<UIRenderTask>
	{
		[[nodiscard]] jlb::StringView GetTextureAtlasFilePath() const override;
		[[nodiscard]] jlb::StringView GetFragmentShaderPath() const override;
		[[nodiscard]] jlb::StringView GetVertexShaderPath() const override;
	};
}
