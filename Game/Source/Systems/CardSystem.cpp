#include "pch.h"
#include "Systems/CardSystem.h"

namespace game
{
	Card CardSystem::GetCard(const size_t index) const
	{
		return _cards[index];
	}
}
