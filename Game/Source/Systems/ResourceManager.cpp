#include "pch.h"
#include "Systems/ResourceManager.h"
#include "StackArray.h"
#include "VkEngine/Graphics/TextureUtils.h"

namespace game
{
	vke::SubTexture ResourceManager::GetSubTexture(const EntitySubTextures type) const
	{
		return _entitySubTextures[static_cast<int32_t>(type)];
	}

	vke::SubTexture ResourceManager::GetSubTexture(const TileSubTextures type) const
	{
		return _tileSubTextures[static_cast<int32_t>(type)];
	}

	vke::SubTexture ResourceManager::GetSubTexture(const UISubTextures type) const
	{
		return _uiSubTextures[static_cast<int32_t>(type)];
	}

	vke::SubTexture ResourceManager::GetSubTexture(const CardSubTextures type) const
	{
		return _cardSubTextures[static_cast<int32_t>(type)];
	}

	void ResourceManager::Allocate(const vke::EngineData& info)
	{
		vke::GameSystem::Allocate(info);

		constexpr auto SUB_TEXTURE_PATH = "Textures/subTextures.dat";
		constexpr auto SUB_TEXTURE_PATH_TILE = "Textures/subTextures-tile.dat";
		constexpr auto SUB_TEXTURE_PATH_UI = "Textures/subTextures-ui.dat";
		constexpr auto SUB_TEXTURE_PATH_CARD = "Textures/subTextures-card.dat";

		constexpr auto ATLAS_LENGTH = 3;
		constexpr auto ATLAS_LENGTH_TILE = 1;
		constexpr auto ATLAS_LENGTH_UI = 9;
		constexpr auto ATLAS_LENGTH_CARD = 6;

#ifdef _DEBUG
		// Entity Render System.
		{
			jlb::StackArray<vke::texture::TextureAtlasPartition, ATLAS_LENGTH> partitions{};
			partitions[0].path = "Textures/humanoid.png";
			partitions[0].width = 3;
			partitions[1].path = "Textures/directional-arrow.png";
			partitions[2].path = "Textures/pickup.png";

			vke::texture::GenerateAtlas(info, "Textures/atlas.png", SUB_TEXTURE_PATH, partitions, 16, 4);	
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
			partitions[1].width = 2;
			partitions[2].path = "Textures/numbers.png";
			partitions[2].width = 10;
			partitions[3].path = "Textures/timeline.png";
			partitions[3].width = 4;
			partitions[4].path = "Textures/timer.png";
			partitions[4].width = 8;
			partitions[5].path = "Textures/timer-fillvalue.png";
			partitions[6].path = "Textures/blank.png";
			partitions[7].path = "Textures/scrollArrow.png";
			partitions[7].width = 2;
			partitions[8].path = "Textures/symbols.png";
			partitions[8].width = 4;

			vke::texture::GenerateAtlas(info, "Textures/atlas-ui.png", SUB_TEXTURE_PATH_UI, partitions, 8, 32);
		}

		// Card Render System.
		{
			jlb::StackArray<vke::texture::TextureAtlasPartition, ATLAS_LENGTH_CARD> partitions{};
			partitions[0].path = "Textures/bash-card.png";
			partitions[0].width = 4;
			partitions[1].path = "Textures/blank-card.png";
			partitions[2].path = "Textures/border-card.png";
			partitions[3].path = "Textures/root-card.png";
			partitions[3].width = 4;
			partitions[4].path = "Textures/fireball-card.png";
			partitions[4].width = 4;
			partitions[5].path = "Textures/idle-card.png";
			partitions[5].width = 4;
			vke::texture::GenerateAtlas(info, "Textures/atlas-card.png", SUB_TEXTURE_PATH_CARD, partitions, 16, 8);
		}

#endif
		auto& allocator = *info.allocator;
		_entitySubTextures.Allocate(allocator, ATLAS_LENGTH);
		_tileSubTextures.Allocate(allocator, ATLAS_LENGTH_TILE);
		_uiSubTextures.Allocate(allocator, ATLAS_LENGTH_UI);
		_cardSubTextures.Allocate(allocator, ATLAS_LENGTH_CARD);
		vke::texture::LoadAtlasSubTextures(SUB_TEXTURE_PATH, _entitySubTextures);
		vke::texture::LoadAtlasSubTextures(SUB_TEXTURE_PATH_TILE, _tileSubTextures);
		vke::texture::LoadAtlasSubTextures(SUB_TEXTURE_PATH_UI, _uiSubTextures);
		vke::texture::LoadAtlasSubTextures(SUB_TEXTURE_PATH_CARD, _cardSubTextures);
	}

	void ResourceManager::Free(const vke::EngineData& info)
	{
		auto& allocator = *info.allocator;
		_cardSubTextures.Free(allocator);
		_uiSubTextures.Free(allocator);
		_tileSubTextures.Free(allocator);
		_entitySubTextures.Free(allocator);
		vke::GameSystem::Free(info);
	}
}
