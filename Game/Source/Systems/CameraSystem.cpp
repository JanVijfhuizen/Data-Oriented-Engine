#include "pch.h"
#include "Systems/CameraSystem.h"
#include "JlbMath.h"
#include "VkEngine/Systems/EntityRenderSystem.h"
#include "VkEngine/Systems/ThreadPoolSystem.h"
#include "VkEngine/Systems/TileRenderSystem.h"

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

			const glm::vec2 offset = target - settings.position - settings.bias;
			const glm::vec2 deadZone = settings.deadZone * .5f;
			const glm::vec2 moveZone = settings.moveZone * .5f;

			assert(deadZone.x > 0 && deadZone.y > 0);
			assert(moveZone.x > 0 && moveZone.y > 0);

			const glm::vec2 deadDelta = jlb::math::Threshold(offset, -deadZone, deadZone);
			const glm::vec2 moveDelta = jlb::math::Threshold(offset, -moveZone, moveZone);

			// Calculate position.
			{
				const glm::vec2 delta = glm::length(moveDelta) > 1e-3f ? moveDelta : deadDelta * info.deltaTime * 1e-2f * settings.moveSpeed;
				sys->settings.position += delta;
			}

			{
				const glm::vec2 zoomZone = settings.zoomZone * .5f;
				const glm::vec2 zoomDelta = jlb::math::Threshold(offset, -zoomZone, zoomZone);
				const glm::vec2 maxZoomZone = moveZone - zoomZone;

				assert(zoomZone.x > 0 && zoomZone.y > 0);
				assert(zoomZone.x < moveZone.x && zoomZone.y < moveZone.y);

				auto zoomCurve = jlb::CreateCurveDecelerate();
				const float fOffsetPct = jlb::math::Max(abs(zoomDelta.x) / maxZoomZone.x, abs(zoomDelta.y) / maxZoomZone.y);
				sys->settings.zoom = 1.f + zoomCurve.Evaluate(fOffsetPct) * sys->settings.zoomMultiplier;
			}
		};
		task.userPtr = this;
		const auto result = threadSys->TryAdd(info, task);
		assert(result != SIZE_MAX);
	}

	void CameraSystem::PostUpdate(const vke::EngineData& info, const jlb::Systems<vke::EngineData> systems)
	{
		System<vke::EngineData>::PostUpdate(info, systems);
		
		const auto entityRenderSys = systems.GetSystem<vke::EntityRenderSystem>();
		const auto tileRenderSys = systems.GetSystem<vke::TileRenderSystem>();

		auto& entityCamera = entityRenderSys->camera;
		entityCamera.position = settings.position;
		entityCamera.pixelSize = settings.pixelSize * settings.zoom;

		auto& tileCamera = tileRenderSys->camera;
		tileCamera.position = settings.position;
		tileCamera.pixelSize = settings.pixelSize * settings.zoom;
	}
}
