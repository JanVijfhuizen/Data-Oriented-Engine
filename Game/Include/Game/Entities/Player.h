#pragma once
#include "StackArray.h"
#include "Systems/MenuSystem.h"

namespace game
{
	struct Player final
	{
		Character character{};
		MenuUpdateInfo menuUpdateInfo{};
		jlb::StackArray<size_t, 5> menuInteractIds{};
	};
}
