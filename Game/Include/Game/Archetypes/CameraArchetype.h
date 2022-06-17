#pragma once
#include "Archetype.h"

namespace game
{
	struct Camera final
	{
		glm::vec2 position{};
		// Size of the pixelart compared to the horizontal length of the window.
		float pixelSize = 0.008f;
	};

	struct CameraUpdateInfo final
	{
		
	};

	class CameraArchetype final : public Archetype<Camera, CameraUpdateInfo>
	{
	private:
		[[nodiscard]] CameraUpdateInfo OnPreEntityUpdate(const EngineData& EngineData, SystemChain& chain) override;
		void OnEntityUpdate(Camera& entity, CameraUpdateInfo& info) override;
	};
}
