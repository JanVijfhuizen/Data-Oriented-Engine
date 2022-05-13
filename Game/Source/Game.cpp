#include "pch.h"
#include "Game.h"
#include "GameState.h"
#include <iostream>

namespace game
{
	void OnKeyInput(const int key, const int action)
	{
		PlayerArchetype::OnKeyInput(key, action, gameState.playerArchetype);
	}

	void OnMouseInput(int key, int action)
	{

	}

	void Start(const EngineOutData outData)
	{
		// Set up archetypes.
		gameState.playerArchetype.Allocate(*outData.allocator, 2);
		gameState.cursorArchetype.Allocate(*outData.allocator, 1);

		// Define resource usage for systems.
		PlayerArchetypeCreateInfo playerArchetypeInfo{};
		playerArchetypeInfo.renderSystem = &gameState.renderSystem;
		playerArchetypeInfo.animationSystem = &gameState.animationSystem;
		gameState.playerArchetype.DefineResourceUsage(playerArchetypeInfo);

		CursorArchetypeCreateInfo cursorArchetypeInfo{};
		cursorArchetypeInfo.renderSystem = &gameState.renderSystem;
		cursorArchetypeInfo.animationSystem = &gameState.animationSystem;
		gameState.cursorArchetype.DefineResourceUsage(cursorArchetypeInfo);

		// Temp.
		gameState.uiSystem.IncreaseRequestedLength(2);

		// Set up systems.
		gameState.animationSystem.Allocate(*outData.allocator);
		gameState.renderSystem.Allocate(outData);
		gameState.uiSystem.Allocate(outData);

		// Start the game.
		gameState.playerArchetype.Start(playerArchetypeInfo);
		gameState.cursorArchetype.Start(cursorArchetypeInfo);

		// Set up game world.
		auto& player1 = gameState.playerArchetype.Add();
		auto& player2 = gameState.playerArchetype.Add();
		gameState.cursorArchetype.Add();
	}

	EngineInData Update(const EngineOutData outData)
	{
		// Update archetypes.
		{
			PlayerArchetypeUpdateInfo playerArchetypeInfo{};
			playerArchetypeInfo.renderSystem = &gameState.renderSystem;
			playerArchetypeInfo.animationSystem = &gameState.animationSystem;
			playerArchetypeInfo.mousePosition = outData.mousePos;
			gameState.playerArchetype.Update(playerArchetypeInfo);
		}

		{
			CursorArchetypeUpdateInfo cursorArchetypeInfo{};
			cursorArchetypeInfo.renderSystem = &gameState.renderSystem;
			cursorArchetypeInfo.animationSystem = &gameState.animationSystem;
			cursorArchetypeInfo.mousePosition = outData.mousePos;
			gameState.cursorArchetype.Update(cursorArchetypeInfo);
		}

		{
			static float f = 0;
			f += outData.deltaTime * .003f;

			// Temp.
			UITask task{};
			task.text = "general kenobi";
			task.spacingPct = abs(sin(f));
			task.leftTop.y = .5;
			gameState.uiSystem.Add(task);
		}

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

		gameState.cursorArchetype.Free(*outData.allocator);
		gameState.playerArchetype.Free(*outData.allocator);
	}
}
