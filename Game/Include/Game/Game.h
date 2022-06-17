#pragma once

#ifdef GAME_EXPORTS
#define GAME_API __declspec(dllexport)
#else
#define GAME_API __declspec(dllimport)
#endif

#include "EngineData.h"
#include "SystemManager.h"

namespace game
{
	extern "C" GAME_API size_t GetSystemCount();
	extern "C" GAME_API void AddSystems(jlb::SystemManager<EngineOutData>& systemManager);
}
