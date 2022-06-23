#pragma once

#ifdef GAME_EXPORTS
#define GAME_API __declspec(dllexport)
#else
#define GAME_API __declspec(dllimport)
#endif

#include "VkEngine/EngineData.h"
#include "SystemManager.h"

namespace game
{
	extern "C" GAME_API void DefineSystems(const jlb::SystemsInitializer<vke::EngineData>& initializer);
}
