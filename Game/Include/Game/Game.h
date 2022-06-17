#pragma once

#ifdef GAME_EXPORTS
#define GAME_API __declspec(dllexport)
#else
#define GAME_API __declspec(dllimport)
#endif

#include "EngineData.h"
#include "SystemManager.h"
#include "SystemInfo.h"

namespace game
{
	extern "C" GAME_API void OnKeyInput(int key, int action);
	extern "C" GAME_API void OnMouseInput(int key, int action);
	extern "C" GAME_API size_t GetSystemCount();
	extern "C" GAME_API void AddSystems(jlb::SystemManager<SystemInfo>& systemManager);
	extern "C" GAME_API void Start(EngineOutData outData);
	extern "C" GAME_API void Update(EngineOutData outData);
	extern "C" GAME_API void OnRecreateSwapChainAssets(EngineOutData outData);
	extern "C" GAME_API void Exit(EngineOutData outData);
}
