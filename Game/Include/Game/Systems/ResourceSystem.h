﻿#pragma once
#include "VkEngine/Systems/GameSystem.h"
#include "VkEngine/Graphics/SubTexture.h"

namespace game
{
	// Simple, straightforward and non scalable resource manager.
	class ResourceSystem final : public vke::GameSystem
	{
	public:
		// Subtextures available for the entity renderer.
		enum class EntitySubTextures
		{
			humanoid,
			pickup
		};

		// Subtextures available for the tile renderer.
		enum class TileSubTextures
		{
			tile
		};

		// Subtextures available for the UI renderer.
		enum class UISubTextures
		{
			alphabet,
			mouse,
			numbers,
			timeline,
			timer,
			timerArrow,
			blank,
			scrollArrow,
			symbols,
			moveArrow
		};

		// Subtextures available for the Card renderer.
		enum class CardSubTextures
		{
			bash,
			blank,
			border,
			root,
			fireball,
			idle
		};

		[[nodiscard]] vke::SubTexture GetSubTexture(EntitySubTextures type) const;
		[[nodiscard]] vke::SubTexture GetSubTexture(TileSubTextures type) const;
		[[nodiscard]] vke::SubTexture GetSubTexture(UISubTextures type) const;
		[[nodiscard]] vke::SubTexture GetSubTexture(CardSubTextures type) const;

	private:
		jlb::Array<vke::SubTexture> _entitySubTextures{};
		jlb::Array<vke::SubTexture> _tileSubTextures{};
		jlb::Array<vke::SubTexture> _uiSubTextures{};
		jlb::Array<vke::SubTexture> _cardSubTextures{};

		void Allocate(const vke::EngineData& info) override;
		void Free(const vke::EngineData& info) override;
	};
}