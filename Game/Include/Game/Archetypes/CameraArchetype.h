#pragma once
#include "Archetype.h"

namespace game
{
	struct Camera final
	{
		glm::vec2 position{};
		float pixelSize = 0.008f;
	};

	struct CameraUpdateInfo final
	{
		
	};

	class CameraArchetype final : public Archetype<Camera, CameraUpdateInfo>
	{
	private:
		[[nodiscard]] CameraUpdateInfo OnPreEntityUpdate(const EngineOutData& outData, SystemChain& chain) override;
		void OnEntityUpdate(Camera& entity, CameraUpdateInfo& info) override;
	};
}
