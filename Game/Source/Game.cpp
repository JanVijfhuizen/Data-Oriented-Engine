#include "pch.h"
#include "Game.h"
#include "GameState.h"

namespace game
{
	void Start(const EngineOutData outData)
	{
		// Set up archetypes.
		gameState.playerArchetype.Allocate(*outData.allocator, 2);

		// Define resource usage for systems.
		PlayerArchetypeInfo playerArchetypeInfo{};
		playerArchetypeInfo.renderSystem = &gameState.renderSystem;
		gameState.playerArchetype.DefineResourceUsage(playerArchetypeInfo);
		// Temp.
		gameState.uiSystem.IncreaseRequestedLength(2);

		// Set up systems.
		gameState.renderSystem.Allocate(outData);
		gameState.uiSystem.Allocate(outData);

		// Set up game world.
		auto& player1 = gameState.playerArchetype.Add();
		auto& player2 = gameState.playerArchetype.Add();
	}

	EngineInData Update(const EngineOutData outData)
	{
		// Update archetypes.
		PlayerArchetypeInfo playerArchetypeInfo{};
		playerArchetypeInfo.renderSystem = &gameState.renderSystem;
		gameState.playerArchetype.Update(playerArchetypeInfo);

		static float f = 0;
		f += outData.deltaTime * .003f;

		// Temp.
		UITask task{};
		task.text = "general kenobi";
		task.spacingPct = abs(sin(f));
		gameState.uiSystem.Add(task);

		// Update systems.
		gameState.renderSystem.Update(outData);
		gameState.uiSystem.Update(outData);

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
		gameState.uiSystem.Free(outData);
		gameState.renderSystem.Free(outData);
		gameState.playerArchetype.Free(*outData.allocator);
	}
}
