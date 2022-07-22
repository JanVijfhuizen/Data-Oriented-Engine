#pragma once
#include "Curve.h"
#include "VkEngine/Systems/GameSystem.h"

namespace game
{
	class CameraSystem final : public vke::GameSystem
	{
	public:
		// Only safe to edit this outside of this system in pre and post update.
		struct Settings final
		{
			glm::vec2 target{};
			glm::vec2 position{};
			float zoom = 0;

			glm::vec2 bias{};
			glm::vec2 deadZone{ 8, 4 };
			glm::vec2 moveZone{ 10 };
			glm::vec2 zoomZone = deadZone;

			float pixelSize = 0.008f;
			float zoomMultiplier = .15f;
			float moveSpeed = 1;
		} settings;

	private:
		void PreUpdate(const vke::EngineData& info, jlb::Systems<vke::EngineData> systems) override;
		void PostUpdate(const vke::EngineData& info, jlb::Systems<vke::EngineData> systems) override;
	};
}
