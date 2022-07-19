#pragma once
#include "VkEngine/Graphics/RenderConventions.h"
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
			glm::vec2 bias{};
			glm::vec2 deadZone{ vke::PIXEL_SIZE_ENTITY * 4 };
			glm::vec2 moveZone{ vke::PIXEL_SIZE_ENTITY * 8 };
			glm::vec2 teleportZone{ vke::PIXEL_SIZE_ENTITY * 12 };
			float pixelSize = 0.008f;
			float zoom = 1;
		} settings;

	private:
		void PreUpdate(const vke::EngineData& info, const jlb::Systems<vke::EngineData> systems) override;
		void PostUpdate(const vke::EngineData& info, const jlb::Systems<vke::EngineData> systems) override;
	};
}
