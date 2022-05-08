#include "pch.h"
#include "Game.h"
#include "GameState.h"
#include <iostream>

namespace game
{
	void OnKeyInput(const int key, const int action)
	{
		
	}

	void Start(const EngineOutData outData)
	{
		// Set up archetypes.
		gameState.playerArchetype.Allocate(*outData.allocator, 2);

		// Define resource usage for systems.
		PlayerArchetypeInfo playerArchetypeInfo{};
		playerArchetypeInfo.renderSystem = &gameState.renderSystem;
		playerArchetypeInfo.animationSystem = &gameState.animationSystem;
		gameState.playerArchetype.DefineResourceUsage(playerArchetypeInfo);

		// Temp.
		gameState.uiSystem.IncreaseRequestedLength(2);

		// Set up systems.
		gameState.animationSystem.Allocate(*outData.allocator);
		gameState.renderSystem.Allocate(outData);
		gameState.uiSystem.Allocate(outData);

		// Start the game.
		gameState.playerArchetype.Start(playerArchetypeInfo);

		// Set up game world.
		auto& player1 = gameState.playerArchetype.Add();
		auto& player2 = gameState.playerArchetype.Add();
	}

	EngineInData Update(const EngineOutData outData)
	{
		// Update archetypes.
		PlayerArchetypeInfo playerArchetypeInfo{};
		playerArchetypeInfo.renderSystem = &gameState.renderSystem;
		playerArchetypeInfo.animationSystem = &gameState.animationSystem;
		gameState.playerArchetype.Update(playerArchetypeInfo);

		static float f = 0;
		f += outData.deltaTime * .003f;

		// Temp.
		UITask task{};
		task.text = "general kenobi";
		task.spacingPct = abs(sin(f));
		task.leftTop.y = .5;
		gameState.uiSystem.Add(task);

		// Update systems.
		gameState.animationSystem.Update(outData);
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
		gameState.animationSystem.Free(*outData.allocator);

		gameState.playerArchetype.Free(*outData.allocator);
	}
}
