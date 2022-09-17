#include "pch.h"
#include "Game.h"
#include "VkEngine/Systems/EntityRenderSystem.h"
#include "VkEngine/Systems/SceneSystem.h"
#include "VkEngine/Systems/UIRenderSystem.h"
#include "VkEngine/Systems/ThreadPoolSystem.h"
#include "VkEngine/Systems/TileRenderSystem.h"

namespace game
{
	void DefineSystems(const jlb::SystemsInitializer<vke::EngineData>& initializer)
	{
		initializer.DefineSystem<vke::ThreadPoolSystem>();
		initializer.DefineSystem<vke::TileRenderSystem>();
		initializer.DefineSystem<vke::EntityRenderSystem>();
		initializer.DefineSystem<vke::UIRenderSystem>();
		initializer.DefineSystem<vke::SceneSystem>();
	}
}
