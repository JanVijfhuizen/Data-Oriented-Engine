#pragma once
#include "Archetype.h"
#include "Entities/Player.h"

namespace game
{
	class PlayerArchetype final : public Archetype<Player>
	{
	public:
		void PreUpdate(const vke::EngineData& info, jlb::Systems<vke::EngineData> systems,
			jlb::ArrayView<Player> entities) override;
		void PostUpdate(const vke::EngineData& info, jlb::Systems<vke::EngineData> systems,
			jlb::ArrayView<Player> entities) override;
		void OnKeyInput(const vke::EngineData& info, jlb::Systems<vke::EngineData> systems, int key, int action);
		void OnMouseInput(const vke::EngineData& info, jlb::Systems<vke::EngineData> systems, int key, int action);

	private:
		bool _wasdKeysInput[4]{};

		static void HandleKeyDirectionInput(int targetKey, int activatedKey, int action, bool& keyPressed);
	};
}
