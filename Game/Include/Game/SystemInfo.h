#pragma once

namespace game
{
	struct EngineOutData;
	class ResourceManager;

	struct SystemInfo final
	{
		EngineOutData const* engineOutData;
		ResourceManager* resourceManager;
	};
}