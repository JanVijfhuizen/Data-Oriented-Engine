#include "pch.h"
#include "Game.h"
#include "GameState.h"
#include "Systems/CollisionSystem.h"
#include "Archetypes/CursorArchetype.h"
#include "Archetypes/PlayerArchetype.h"
#include "Systems/MovementSystem.h"
#include "Systems/TextRenderSystem.h"
#include "Archetypes/WallArchetype.h"
#include "Systems/LineRenderSystem.h"

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
		auto& chain = gameState.chain;

		chain.Get<CameraArchetype>()->IncreaseRequestedLength(1);
		chain.Get<WallArchetype>()->IncreaseRequestedLength(1);
		chain.Get<LineRenderSystem>()->IncreaseRequestedLength(2);
	}

	void GameStart(const EngineOutData& outData)
	{
		auto& chain = gameState.chain;

		// Set up game world.
		chain.Get<CameraArchetype>()->Add();
		chain.Get<PlayerArchetype>()->Add();
		chain.Get<CursorArchetype>()->Add();
		auto& wall = gameState.chain.Get<WallArchetype>()->Add();
		wall.transform.position = { 40, 70 };
	}

	void GameUpdate(const EngineOutData& outData)
	{
		return;
		static float f = 0;
		f += outData.deltaTime / 1000;

		auto& camera = (*gameState.chain.Get<CameraArchetype>())[0];
		camera.pixelSize = 0.008f + 0.004f * sin(f);
		camera.position = { sin(f) * 64, cos(f) * 64 };

		TextRenderTask task{};
		task.text = "general kenobi";
		task.spacingPct = abs(sin(f));
		gameState.chain.Get<TextRenderSystem>()->AddAsCharRenderTasks(task);

		auto lineRenderSystem = gameState.chain.Get<LineRenderSystem>();
		auto& lineTask = lineRenderSystem->Add();
		lineTask.start = { 0, 0 };
		lineTask.end = {sin(f) * 50, cos(f) * 100};
		auto& lineTask2 = lineRenderSystem->Add();
		lineTask2.start = { sin(f) * 100 + 200, 50 * sin(f * 2) };
		lineTask2.end = { sin(f) * 64, cos(f) * 32 };
	}

	void Start(const EngineOutData outData)
	{
		// Add archetypes.
		auto& chain = gameState.chain;
		chain.Add<CameraArchetype>(outData);
		chain.Add<PlayerArchetype>(outData);
		chain.Add<WallArchetype>(outData);
		chain.Add<CursorArchetype>(outData);

		// Add systems.
		chain.Add<MovementSystem>(outData);
		chain.Add<CollisionSystem>(outData);
		chain.Add<AnimationSystem>(outData);
		chain.Add<EntityRenderSystem>(outData);
		chain.Add<TextRenderSystem>(outData);
		chain.Add<LineRenderSystem>(outData);

		DefineUsage(outData);
		chain.Allocate(outData);

		// Start the game.
		chain.Awake(outData);
		GameStart(outData);
		chain.Start(outData);
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
		gameState.chain.RecreateSwapChainAssets(outData);
	}

	void Exit(const EngineOutData outData)
	{
		gameState.chain.Free(outData);
	}
}
