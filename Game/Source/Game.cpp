#include "pch.h"
#include "Game.h"
#include "GameState.h"

namespace game
{
	void OnKeyInput(const int key, const int action)
	{
		PlayerArchetype::OnKeyInput(key, action, gameState.playerArchetype);
	}

	void OnMouseInput(const int key, const int action)
	{
		CursorArchetype::OnMouseKeyInput(key, action, gameState.cursorArchetype);
	}

	void GameStart(const EngineOutData& outData)
	{
		// Set up game world.
		auto& player1 = gameState.playerArchetype.Add();
		auto& player2 = gameState.playerArchetype.Add();
		gameState.cursorArchetype.Add();
	}

	void GameUpdate(const EngineOutData& outData)
	{
		static float f = 0;
		f += outData.deltaTime * .003f;

		// Temp.
		TextTask task{};
		task.text = "general kenobi";
		task.spacingPct = abs(sin(f));
		task.leftTop.y = .5;
		gameState.uiSystem.Add(task);
	}

	void Start(const EngineOutData outData)
	{
		auto& allocator = *outData.allocator;

		// Set up archetypes.
		gameState.playerArchetype.Allocate(allocator, 2);
		gameState.cursorArchetype.Allocate(allocator, 1);

		// Define resource usage for systems.
		PlayerArchetypeCreateInfo playerArchetypeInfo{};
		playerArchetypeInfo.renderSystem = &gameState.renderSystem;
		playerArchetypeInfo.animationSystem = &gameState.animationSystem;
		playerArchetypeInfo.movementSystem = &gameState.movementSystem;
		gameState.playerArchetype.DefineResourceUsage(playerArchetypeInfo);

		CursorArchetypeCreateInfo cursorArchetypeInfo{};
		cursorArchetypeInfo.renderSystem = &gameState.renderSystem;
		cursorArchetypeInfo.animationSystem = &gameState.animationSystem;
		gameState.cursorArchetype.DefineResourceUsage(cursorArchetypeInfo);

		// Temp.
		gameState.uiSystem.IncreaseRequestedLength(2);

		// Set up systems.
		gameState.movementSystem.Allocate(allocator);
		gameState.collisionSystem.Allocate(allocator);

		gameState.animationSystem.Allocate(allocator);
		gameState.renderSystem.Allocate(outData);
		gameState.uiSystem.Allocate(outData);

		// Start the game.
		gameState.playerArchetype.Start(playerArchetypeInfo);
		gameState.cursorArchetype.Start(cursorArchetypeInfo);

		GameStart(outData);
	}

	EngineInData Update(const EngineOutData outData)
	{
		// Update archetypes.
		{
			PlayerArchetypeUpdateInfo playerArchetypeInfo{};
			playerArchetypeInfo.renderSystem = &gameState.renderSystem;
			playerArchetypeInfo.animationSystem = &gameState.animationSystem;
			playerArchetypeInfo.mousePosition = outData.mousePos;
			playerArchetypeInfo.movementSystem = &gameState.movementSystem;
			gameState.playerArchetype.Update(playerArchetypeInfo);
		}

		{
			CursorArchetypeUpdateInfo cursorArchetypeInfo{};
			cursorArchetypeInfo.renderSystem = &gameState.renderSystem;
			cursorArchetypeInfo.animationSystem = &gameState.animationSystem;
			cursorArchetypeInfo.mousePosition = outData.mousePos;
			gameState.cursorArchetype.Update(cursorArchetypeInfo);
		}

		GameUpdate(outData);

		// Update game systems.
		gameState.movementSystem.Update();
		gameState.collisionSystem.Update();

		// Update graphic systems.
		gameState.animationSystem.Update(outData);
		gameState.renderSystem.Update(outData, {});
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
		auto& allocator = *outData.allocator;

		gameState.collisionSystem.Free(allocator);
		gameState.movementSystem.Free(allocator);

		gameState.uiSystem.Free(outData);
		gameState.renderSystem.Free(outData);
		gameState.animationSystem.Free(allocator);

		gameState.cursorArchetype.Free(allocator);
		gameState.playerArchetype.Free(allocator);
	}
}
