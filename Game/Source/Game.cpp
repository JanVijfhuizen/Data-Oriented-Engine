#include "pch.h"
#include "Game.h"
#include "GameState.h"

namespace game
{
	void Start(const EngineOutData outData)
	{
		gameState = {};
		// Set up archetypes.
		gameState.playerArchetype.Allocate(*outData.allocator, 1);
		// Set up systems.
		gameState.renderSystem.Allocate(outData, 1);

		// Set up game world.
		auto& player1 = gameState.playerArchetype.Add();
	}

	EngineInData Update(const EngineOutData outData)
	{
		// Update archetypes.
		gameState.playerArchetype.Update(gameState.renderSystem);
		// Update systems.
		gameState.renderSystem.Update(*outData.app);

		auto& player = gameState.playerArchetype[0];
		jlb::Get<PlayerArchetype::Transform>(player).position.x = sin(outData.time * 0.001f) * 4;

		EngineInData inData{};
		return inData;
	}

	void OnRecreateSwapChainAssets(const EngineOutData outData)
	{
		
	}

	void Exit(EngineOutData outData)
	{
		gameState.renderSystem.Free(*outData.allocator);
		gameState.playerArchetype.Free(*outData.allocator);
	}
}
