#pragma once

#ifdef GAME_EXPORTS
#define GAME_API __declspec(dllexport)
#else
#define GAME_API __declspec(dllimport)
#endif
#include "EngineData.h"

namespace game
{
	extern "C" GAME_API void Start(EngineOutData outData);
	extern "C" GAME_API EngineInData Update(EngineOutData outData);
	extern "C" GAME_API void OnRecreateSwapChainAssets(EngineOutData outData);
	extern "C" GAME_API void Exit(EngineOutData outData);
}
