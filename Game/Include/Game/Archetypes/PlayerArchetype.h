#pragma once
#include "CharacterArchetype.h"
#include "Entities/Player.h"
#include "Systems/CardPreviewSystem.h"
#include "Systems/MenuSystem.h"

namespace game
{
	// Handles player character behaviour and input.
	class PlayerArchetype final : public CharacterArchetype<Player>
	{
		struct Input final
		{
			bool pressed = false;
			bool pressedSinceStartOfFrame = false;
			bool valid = false;
		};

		enum class MenuIndex
		{
			main,
			inventory,
			deck
		};

		Input _movementInput[4]{};
		const float _scalingOnSelected = 0.5f;

		CardPreviewTaskUpdateInfo _cardPreviewUpdateInfo{};
		MenuTaskUpdateInfo _menuUpdateInfo{};
		jlb::StackArray<size_t, 6> _menuInteractIds{};
		MenuTaskUpdateInfo _secondMenuUpdateInfo{};
		jlb::StackArray<size_t, 6> _secondMenuInteractIds{};
		MenuIndex _menuIndex = MenuIndex::main;
		size_t _cardHovered = SIZE_MAX;
		size_t _cardActivated = SIZE_MAX;
		size_t _menuTaskId = SIZE_MAX;
		size_t _secondMenuTaskId = SIZE_MAX;
		size_t _cardPreviewTaskId = SIZE_MAX;

		void OnPreUpdate(const EntityArchetypeInfo& info, jlb::Systems<EntityArchetypeInfo> archetypes,
		                 jlb::NestedVector<Player>& entities) override;
		void OnPostUpdate(const EntityArchetypeInfo& info, jlb::Systems<EntityArchetypeInfo> archetypes, 
			jlb::NestedVector<Player>& entities) override;

		void OnKeyInput(const EntityArchetypeInfo& info, jlb::Systems<EntityArchetypeInfo> systems, int key,
			int action) override;

		[[nodiscard]] vke::SubTexture DefineSubTextureSet(const vke::EngineData& info, jlb::Systems<vke::EngineData> systems) override;
		static void HandleKeyDirectionInput(int targetKey, int activatedKey, int action, Input& input, Input& opposite);
		void Reset();
	};
}
