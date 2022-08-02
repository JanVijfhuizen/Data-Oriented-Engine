#include "pch.h"
#include "Systems/CardSystem.h"

namespace game
{
	Card CardSystem::GetCard(const size_t index) const
	{
		return _cards[index];
	}

	void CardSystem::Allocate(const vke::EngineData& info)
	{
		System<vke::EngineData>::Allocate(info);
		_cards.Allocate(*info.allocator, 1);
		_cards[0].name = "fireball";
	}

	void CardSystem::Free(const vke::EngineData& info)
	{
		_cards.Free(*info.allocator);
		System<vke::EngineData>::Free(info);
	}
}
