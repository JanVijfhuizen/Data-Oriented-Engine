#include "pch.h"
#include "Game.h"
#include "GameState.h"
#include "Systems/CollisionSystem.h"

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
		
	}

	void Start(const EngineOutData outData)
	{
		auto& allocator = *outData.allocator;
		auto& chain = gameState.chain;

		chain.Add<MovementSystem>(outData);
		chain.Add<CollisionSystem>(outData);
		chain.Add<AnimationSystem>(outData);
		chain.Add<RenderSystem<RenderTask>>(outData);
		chain.Add<TextSystem>(outData);

		// Set up archetypes.
		gameState.playerArchetype.Allocate(allocator, 2);
		gameState.cursorArchetype.Allocate(allocator, 1);

		// Define resource usage for systems.
		PlayerArchetypeCreateInfo playerArchetypeInfo{};
		playerArchetypeInfo.renderSystem = gameState.chain.Get<EntityRenderSystem>();
		playerArchetypeInfo.animationSystem = gameState.chain.Get<AnimationSystem>();
		playerArchetypeInfo.movementSystem = gameState.chain.Get<MovementSystem>();
		gameState.playerArchetype.DefineResourceUsage(playerArchetypeInfo);

		CursorArchetypeCreateInfo cursorArchetypeInfo{};
		cursorArchetypeInfo.renderSystem = gameState.chain.Get<EntityRenderSystem>();
		cursorArchetypeInfo.animationSystem = gameState.chain.Get<AnimationSystem>();
		gameState.cursorArchetype.DefineResourceUsage(cursorArchetypeInfo);

		chain.Allocate(outData);

		// Start the game.
		gameState.playerArchetype.Start(playerArchetypeInfo);
		gameState.cursorArchetype.Start(cursorArchetypeInfo);

		chain.Start(outData);

		GameStart(outData);
	}

	EngineInData Update(const EngineOutData outData)
	{
		// Update archetypes.
		{
			PlayerArchetypeUpdateInfo playerArchetypeInfo{};
			playerArchetypeInfo.renderSystem = gameState.chain.Get<EntityRenderSystem>();
			playerArchetypeInfo.animationSystem = gameState.chain.Get<AnimationSystem>();
			playerArchetypeInfo.mousePosition = outData.mousePos;
			playerArchetypeInfo.movementSystem = gameState.chain.Get<MovementSystem>();
			gameState.playerArchetype.Update(playerArchetypeInfo);
		}

		{
			CursorArchetypeUpdateInfo cursorArchetypeInfo{};
			cursorArchetypeInfo.renderSystem = gameState.chain.Get<EntityRenderSystem>();
			cursorArchetypeInfo.animationSystem = gameState.chain.Get<AnimationSystem>();
			cursorArchetypeInfo.mousePosition = outData.mousePos;
			gameState.cursorArchetype.Update(cursorArchetypeInfo);
		}

		GameUpdate(outData);

		gameState.chain.Update(outData);

		EngineInData inData{};
		return inData;
	}

	void OnRecreateSwapChainAssets(const EngineOutData outData)
	{
		auto renderSystem = gameState.chain.Get<EntityRenderSystem>();
		renderSystem->DestroySwapChainAssets(outData);
		renderSystem->CreateSwapChainAssets(outData);
		auto textSystem = gameState.chain.Get<TextSystem>();
		textSystem->DestroySwapChainAssets(outData);
		textSystem->CreateSwapChainAssets(outData);
	}

	void Exit(const EngineOutData outData)
	{
		auto& allocator = *outData.allocator;

		gameState.chain.Free(outData);

		gameState.cursorArchetype.Free(allocator);
		gameState.playerArchetype.Free(allocator);
	}
}
