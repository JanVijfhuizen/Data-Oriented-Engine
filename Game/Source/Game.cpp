#include "pch.h"
#include "Game.h"
#include "GameState.h"
#include "Systems/CollisionSystem.h"
#include "Archetypes/CursorArchetype.h"
#include "Archetypes/PlayerArchetype.h"
#include "Systems/MovementSystem.h"
#include "Systems/TextRenderSystem.h"
#include "Archetypes/WallArchetype.h"

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

	void DefineUsage(const EngineOutData& outData)
	{
		gameState.chain.Get<WallArchetype>()->IncreaseRequestedLength(1);
	}

	void GameStart(const EngineOutData& outData)
	{
		// Set up game world.
		gameState.chain.Get<PlayerArchetype>()->Add();
		gameState.chain.Get<CursorArchetype>()->Add();
		auto& wall = gameState.chain.Get<WallArchetype>()->Add();
		wall.transform.position = { 40, 70 };
	}

	void GameUpdate(const EngineOutData& outData)
	{
		static float f = 0;
		f += 0.001f;

		TextRenderTask task{};
		task.text = "general kenobi";
		task.spacingPct = abs(sin(f));
		gameState.chain.Get<TextRenderSystem>()->AddAsCharRenderTasks(task);
	}

	void Start(const EngineOutData outData)
	{
		// Add archetypes.
		auto& chain = gameState.chain;
		chain.Add<PlayerArchetype>(outData);
		chain.Add<WallArchetype>(outData);
		chain.Add<CursorArchetype>(outData);

		// Add systems.
		chain.Add<MovementSystem>(outData);
		chain.Add<CollisionSystem>(outData);
		chain.Add<AnimationSystem>(outData);
		chain.Add<EntityRenderSystem>(outData);
		chain.Add<TextRenderSystem>(outData);

		DefineUsage(outData);
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
