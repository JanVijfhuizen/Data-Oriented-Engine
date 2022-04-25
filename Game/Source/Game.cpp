#include "pch.h"
#include "Game.h"
#include "GameState.h"

namespace game
{
	void Start(const EngineOutData outData)
	{
		gameState = {};
		// Set up archetypes.
		gameState.playerArchetype.Allocate(*outData.allocator, 2);

		// Define resource usage for systems.
		gameState.playerArchetype.DefineResourceUsage(gameState.renderSystem);

		// Set up systems.
		gameState.renderSystem.Allocate(outData);

		// Set up game world.
		auto& player1 = gameState.playerArchetype.Add();
		auto& player2 = gameState.playerArchetype.Add();
	}

	EngineInData Update(const EngineOutData outData)
	{
		// Update archetypes.
		gameState.playerArchetype.Update(gameState.renderSystem);
		// Update systems.
		gameState.renderSystem.Update(outData);

		auto& player = gameState.playerArchetype[1];
		jlb::Get<PlayerArchetype::Transform>(player).position.x = sin(outData.time * 0.001f) * 4;

		EngineInData inData{};
		return inData;
	}

	void OnRecreateSwapChainAssets(const EngineOutData outData)
	{
		gameState.renderSystem.DestroySwapChainAssets(outData);
		gameState.renderSystem.CreateSwapChainAssets(outData);
	}

	void Exit(const EngineOutData outData)
	{
		gameState.renderSystem.Free(outData);
		gameState.playerArchetype.Free(*outData.allocator);
	}
}
