#include "pch.h"
#include "Archetypes/CameraArchetype.h"

namespace game
{
	CameraUpdateInfo CameraArchetype::OnPreEntityUpdate(const EngineOutData& outData, SystemChain& chain)
	{
		return {};
	}

	void CameraArchetype::OnEntityUpdate(Camera& entity, CameraUpdateInfo& info)
	{
	}
}
