#include "pch.h"
#include "Game.h"
#include "GameState.h"
#include "Systems/CollisionSystem.h"
#include "Archetypes/CursorArchetype.h"
#include "Archetypes/PlayerArchetype.h"
#include "Systems/MovementSystem.h"

namespace game
{
	void OnKeyInput(const int key, const int action)
	{
		PlayerArchetype::OnKeyInput(key, action, *gameState.chain.Get<PlayerArchetype>());
	}

	void OnMouseInput(const int key, const int action)
	{
		CursorArchetype::OnMouseKeyInput(key, action, *gameState.chain.Get<CursorArchetype>());
	}

	void GameStart(const EngineOutData& outData)
	{
		// Set up game world.
		auto playerArchetype = gameState.chain.Get<PlayerArchetype>();
		auto& player1 = playerArchetype->Add();
		auto& player2 = playerArchetype->Add();
		gameState.chain.Get<CursorArchetype>()->Add();
	}

	void GameUpdate(const EngineOutData& outData)
	{
		
	}

	void Start(const EngineOutData outData)
	{
		auto& chain = gameState.chain;

		// Add archetypes.
		chain.Add<PlayerArchetype>(outData);
		chain.Add<CursorArchetype>(outData);

		// Add systems.
		chain.Add<MovementSystem>(outData);
		chain.Add<CollisionSystem>(outData);
		chain.Add<AnimationSystem>(outData);
		chain.Add<RenderSystem<RenderTask>>(outData);

		chain.Allocate(outData);

		// Start the game.
		chain.Start(outData);
		GameStart(outData);
	}

	EngineInData Update(const EngineOutData outData)
	{
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
	}

	void Exit(const EngineOutData outData)
	{
		gameState.chain.Free(outData);
	}
}
