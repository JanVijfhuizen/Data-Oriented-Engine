#pragma once
#include "CharacterArchetype.h"
#include "Entities/Player.h"

namespace game
{
	// Handles player character behaviour and input.
	class PlayerArchetype final : public CharacterArchetype<Player>
	{
	public:
		void PreUpdate(const vke::EngineData& info, jlb::Systems<vke::EngineData> systems,
			jlb::ArrayView<Player> entities) override;
		void EndFrame(const vke::EngineData& info, jlb::Systems<vke::EngineData> systems,
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

		Input _movementInput[4]{};
		const float _scalingOnSelected = 0.5f;

		static void HandleKeyDirectionInput(int targetKey, int activatedKey, int action, Input& input, Input& opposite);
	};
}
