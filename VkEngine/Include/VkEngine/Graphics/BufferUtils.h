#pragma once

namespace vke
{
	struct Buffer;

	namespace buffer
	{
		void Free(const EngineData& info, const Buffer& buffer);
	}
}
