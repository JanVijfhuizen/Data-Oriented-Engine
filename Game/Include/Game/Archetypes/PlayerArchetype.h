#pragma once
#include "CharacterArchetype.h"
#include "Entities/Player.h"
#include "Systems/MenuSystem.h"

namespace game
{
	// Handles player character behaviour and input.
	class PlayerArchetype final : public CharacterArchetype<Player>
	{
	public:
		void PreUpdate(const vke::EngineData& info, jlb::Systems<vke::EngineData> systems,
			jlb::ArrayView<Player> entities) override;
		void PostUpdate(const vke::EngineData& info, jlb::Systems<vke::EngineData> systems,
			jlb::ArrayView<Player> entities) override;
		void OnKeyInput(const vke::EngineData& info, jlb::Systems<vke::EngineData> systems, int key, int action);
		void OnMouseInput(const vke::EngineData& info, jlb::Systems<vke::EngineData> systems, int key, int action);

	private:
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
		float _animLerp = 0;
		float _animSpeed = 5;

		MenuUpdateInfo _menuUpdateInfo{};
		jlb::StackArray<size_t, 6> _menuInteractIds{};
		MenuUpdateInfo _secondMenuUpdateInfo{};
		jlb::StackArray<size_t, 6> _secondMenuInteractIds{};
		MenuIndex _menuIndex = MenuIndex::main;
		size_t _cardHovered = SIZE_MAX;
		size_t _cardActivated = SIZE_MAX;

		static void HandleKeyDirectionInput(int targetKey, int activatedKey, int action, Input& input, Input& opposite);
	};
}
