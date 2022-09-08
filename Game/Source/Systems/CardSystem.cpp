#include "pch.h"
#include "Systems/CardSystem.h"
#include "Systems/ResourceSystem.h"

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
	}

	void CardSystem::Start(const vke::EngineData& info, const jlb::Systems<vke::EngineData> systems)
	{
		System<vke::EngineData>::Start(info, systems);

		const auto resourceSys = systems.Get<ResourceSystem>();

		// Temp.
		_cards[0].name = "fireball";
		_cards[0].art = resourceSys->GetSubTexture(ResourceSystem::CardSubTextures::fireball);
		_cards[0].cost = 2;
		_cards[0].text = "deal 4 damage. keeps spreading to adjacent tiles, with the damage halved every time.";
		_cards[1].name = "root";
		_cards[1].art = resourceSys->GetSubTexture(ResourceSystem::CardSubTextures::root);
		_cards[1].cost = 2;
		_cards[1].text = "restrict movement for 2 turns.";
		_cards[2].name = "bash";
		_cards[2].art = resourceSys->GetSubTexture(ResourceSystem::CardSubTextures::bash);
		_cards[2].text = "deal 3 damage.";
	}

	void CardSystem::Free(const vke::EngineData& info)
	{
		_cards.Free(*info.allocator);
		System<vke::EngineData>::Free(info);
	}
}
