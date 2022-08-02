#include "pch.h"
#include "Systems/CardSystem.h"

#include "Systems/ResourceManager.h"

namespace game
{
	Card CardSystem::GetCard(const size_t index) const
	{
		return _cards[index];
	}

	void CardSystem::Allocate(const vke::EngineData& info)
	{
		System<vke::EngineData>::Allocate(info);
		_cards.Allocate(*info.allocator, 3);
		_cards[0].name = "fireball";
		_cards[1].name = "root";
		_cards[2].name = "bash";
	}

	void CardSystem::Start(const vke::EngineData& info, const jlb::Systems<vke::EngineData> systems)
	{
		System<vke::EngineData>::Start(info, systems);

		const auto resourceSys = systems.GetSystem<ResourceManager>();
		const auto bashArt = resourceSys->GetSubTexture(ResourceManager::CardSubTextures::bash);

		// Temp.
		_cards[0].art = bashArt;
		_cards[0].animLength = 2;
		_cards[1].art = bashArt;
		_cards[1].animLength = 3;
		_cards[2].art = bashArt;
		_cards[2].animLength = 4;
	}

	void CardSystem::Free(const vke::EngineData& info)
	{
		_cards.Free(*info.allocator);
		System<vke::EngineData>::Free(info);
	}
}
