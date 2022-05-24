#pragma once
#include "Buffer.h"

namespace game
{
	struct Mesh final
	{
		Buffer vertexBuffer;
		Buffer indexBuffer;
		size_t indexCount;
	};
}
