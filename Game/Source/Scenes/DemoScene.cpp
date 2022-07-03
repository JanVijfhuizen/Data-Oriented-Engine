#include "pch.h"
#include "Scenes/DemoScene.h"
#include "VkEngine/Systems/EntityRenderSystem.h"

namespace game::demo
{
	void DemoScene::Update(const vke::EngineData& info, const jlb::Systems<vke::EngineData> systems)
	{
		GameScene::Update(info, systems);

		const auto sys = systems.GetSystem<vke::EntityRenderSystem>();

		for (size_t y = 0; y < 32; ++y)
			for (size_t x = 0; x < 32; ++x)
			{
				vke::EntityRenderTask task{};
				task.subTexture.rBot = { 0.5f, 1 };
				task.transform = {};
				task.transform.position = glm::vec2(x, y) * static_cast<float>(vke::PIXEL_SIZE_ENTITY);
				const auto result = sys->TryAdd(task);
				assert(result);
			}
	}
}
