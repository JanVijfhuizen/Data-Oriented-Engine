#pragma once
#include "Buffer.h"

namespace vke
{
	struct Mesh final
	{
		Buffer vertexBuffer{};
		Buffer indexBuffer{};
		size_t indexCount = 0;
	};
}
