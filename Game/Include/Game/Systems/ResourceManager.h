#pragma once
#include "VkEngine/Systems/GameSystem.h"
#include "VkEngine/Graphics/SubTexture.h"

namespace game
{
	// Simple, straightforward and non scalable resource manager.
	class ResourceManager final : public vke::GameSystem
	{
	public:
		enum class EntitySubTextures
		{
			mouse,
			humanoid,
			tile
		};

		enum class UISubTextures
		{
			alphabet,
			mouse,
			timeline
		};

		[[nodiscard]] vke::SubTexture GetSubTexture(EntitySubTextures type) const;
		[[nodiscard]] vke::SubTexture GetSubTexture(UISubTextures type) const;

	private:
		jlb::Array<vke::SubTexture> _entitySubTextures{};
		jlb::Array < vke::SubTexture> _uiSubTextures{};

		void Allocate(const vke::EngineData& info) override;
		void Free(const vke::EngineData& info) override;
	};
}
