#include "pch.h"
#include "Systems/CameraSystem.h"
#include "JlbMath.h"
#include "VkEngine/Systems/EntityRenderSystem.h"
#include "VkEngine/Systems/ThreadPoolSystem.h"

namespace game 
{
	void CameraSystem::PreUpdate(const vke::EngineData& info, const jlb::Systems<vke::EngineData> systems)
	{
		System<vke::EngineData>::PreUpdate(info, systems);

		const auto threadSys = systems.GetSystem<vke::ThreadPoolSystem>();

		vke::ThreadPoolTask task{};
		task.func = [](const vke::EngineData& info, const jlb::Systems<vke::EngineData> systems, void* userPtr)
		{
			const auto sys = reinterpret_cast<CameraSystem*>(userPtr);
			const auto& settings = sys->settings;
			const auto& target = settings.target;

			const glm::vec2 offset = target - settings.position;
			const glm::vec2 moveZone = settings.moveZone * .5f;

			glm::vec2 delta = jlb::math::Threshold(offset, -moveZone, moveZone);
			sys->settings.position += delta;
		};
		task.userPtr = this;
		const auto result = threadSys->TryAdd(info, task);
		assert(result != SIZE_MAX);
	}

	void CameraSystem::PostUpdate(const vke::EngineData& info, const jlb::Systems<vke::EngineData> systems)
	{
		System<vke::EngineData>::PostUpdate(info, systems);

		const auto entityRenderSys = systems.GetSystem<vke::EntityRenderSystem>();
		auto& camera = entityRenderSys->camera;
		camera.position = settings.position;
		camera.pixelSize = settings.pixelSize * settings.zoom;
	}
}
