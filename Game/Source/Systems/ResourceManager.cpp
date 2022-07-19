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

	vke::SubTexture ResourceManager::GetSubTexture(TileSubTextures type) const
	{
		return _tileSubTextures[static_cast<int32_t>(type)];
	}

	vke::SubTexture ResourceManager::GetSubTexture(UISubTextures type) const
	{
		return _uiSubTextures[static_cast<int32_t>(type)];
	}

	void ResourceManager::Allocate(const vke::EngineData& info)
	{
		vke::GameSystem::Allocate(info);

		constexpr auto SUB_TEXTURE_PATH = "Textures/subTextures.dat";
		constexpr auto SUB_TEXTURE_PATH_TILE = "Textures/subTextures-tile.dat";
		constexpr auto SUB_TEXTURE_PATH_UI = "Textures/subTextures-ui.dat";

		constexpr auto ATLAS_LENGTH = 2;
		constexpr auto ATLAS_LENGTH_TILE = 1;
		constexpr auto ATLAS_LENGTH_UI = 4;

#ifdef _DEBUG
		// Entity Render System.
		{
			jlb::StackArray<vke::texture::TextureAtlasPartition, ATLAS_LENGTH> partitions{};
			partitions[0].path = "Textures/mouse.png";
			partitions[1].path = "Textures/humanoid.png";
			partitions[1].width = 2;

			vke::texture::GenerateAtlas(info, "Textures/atlas.png", SUB_TEXTURE_PATH, partitions, 8, 4);	
		}

		// Tile Render System.
		{
			jlb::StackArray<vke::texture::TextureAtlasPartition, ATLAS_LENGTH_TILE> partitions{};
			partitions[0].path = "Textures/tile.png";
			vke::texture::GenerateAtlas(info, "Textures/atlas-tile.png", SUB_TEXTURE_PATH_TILE, partitions, 8, 1);
		}

		// UI Render System.
		{
			jlb::StackArray<vke::texture::TextureAtlasPartition, ATLAS_LENGTH_UI> partitions{};
			partitions[0].path = "Textures/alphabet.png";
			partitions[0].width = 26;
			partitions[1].path = "Textures/mouse.png";
			partitions[2].path = "Textures/numbers.png";
			partitions[2].width = 10;
			partitions[3].path = "Textures/timeline.png";
			partitions[3].width = 4;

			vke::texture::GenerateAtlas(info, "Textures/atlas-ui.png", SUB_TEXTURE_PATH_UI, partitions, 8, 32);
		}
#endif

		_entitySubTextures.Allocate(*info.allocator, ATLAS_LENGTH);
		_tileSubTextures.Allocate(*info.allocator, ATLAS_LENGTH_TILE);
		_uiSubTextures.Allocate(*info.allocator, ATLAS_LENGTH_UI);
		vke::texture::LoadAtlasSubTextures(SUB_TEXTURE_PATH, _entitySubTextures);
		vke::texture::LoadAtlasSubTextures(SUB_TEXTURE_PATH_TILE, _tileSubTextures);
		vke::texture::LoadAtlasSubTextures(SUB_TEXTURE_PATH_UI, _uiSubTextures);
	}

	void ResourceManager::Free(const vke::EngineData& info)
	{
		_uiSubTextures.Free(*info.allocator);
		_tileSubTextures.Free(*info.allocator);
		_entitySubTextures.Free(*info.allocator);
		vke::GameSystem::Free(info);
	}
}
