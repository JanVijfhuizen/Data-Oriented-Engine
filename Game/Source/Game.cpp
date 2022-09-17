#include "pch.h"
#include "Game.h"

#include "VkEngine/Systems/SceneSystem.h"
#include "VkEngine/Systems/ThreadPoolSystem.h"

namespace game
{
	class GameSceneSystem final : public vke::SceneSystem
	{
	protected:
		void DefineScenes(const jlb::SystemsInitializer<vke::EngineData>& initializer) override
		{
			
		}
	};

	void DefineSystems(const jlb::SystemsInitializer<vke::EngineData>& initializer)
	{
		initializer.DefineSystem<vke::ThreadPoolSystem>();
		initializer.DefineSystem<GameSceneSystem>();
	}
}
