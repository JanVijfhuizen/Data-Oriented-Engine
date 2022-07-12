#include "pch.h"
#include "Systems/ResourceManager.h"
#include "StackArray.h"
#include "VkEngine/Graphics/TextureUtils.h"

namespace game
{
	vke::SubTexture ResourceManager::GetSubTexture(EntitySubTextures type) const
	{
		return _entitySubTextures[static_cast<int32_t>(type)];
	}

	vke::SubTexture ResourceManager::GetSubTexture(UISubTextures type) const
	{
		return _uiSubTextures[static_cast<int32_t>(type)];
	}

	void ResourceManager::Allocate(const vke::EngineData& info)
	{
		constexpr auto SUB_TEXTURE_PATH = "Textures/SubTextures.dat";

		// Testing.
		jlb::StackArray<vke::texture::TextureAtlasPartition, 3> partitions{};
		partitions[0].path = "Textures/mouse.png";
		partitions[1].path = "Textures/humanoid.png";
		partitions[1].width = 2;
		partitions[2].path = "Textures/tile.png";

		vke::texture::GenerateAtlas(info, "Textures/Atlas.png", SUB_TEXTURE_PATH, partitions, 8, 4);

		_entitySubTextures.Allocate(*info.allocator, partitions.GetLength());
		vke::texture::LoadAtlasSubTextures(SUB_TEXTURE_PATH, _entitySubTextures);
	}

	void ResourceManager::Free(const vke::EngineData& info)
	{
		_entitySubTextures.Free(*info.allocator);
	}
}
