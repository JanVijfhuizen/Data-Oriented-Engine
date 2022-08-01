#pragma once
#include "StackArray.h"
#include "Systems/MenuSystem.h"

namespace game
{
	struct Player final
	{
		enum class MenuIndex
		{
			main,
			cards
		};

		Character character{};
		MenuUpdateInfo menuUpdateInfo{};
		jlb::StackArray<size_t, 4> menuInteractIds{};
		MenuIndex menuIndex = MenuIndex::main;
	};
}
