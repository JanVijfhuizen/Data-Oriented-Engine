#pragma once

#ifdef GAME_EXPORTS
#define GAME_API __declspec(dllexport)
#else
#define GAME_API __declspec(dllimport)
#endif

namespace game
{
	extern "C" GAME_API void Start();
	extern "C" GAME_API void Update();
}