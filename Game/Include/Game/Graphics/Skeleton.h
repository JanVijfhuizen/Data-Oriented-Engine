#pragma once
#include "Array.h"
#include "SubTexture.h"

namespace game
{
	class Skeleton final
	{
	public:
		enum class Type
		{
			human
		} type;

		jlb::Array<SubTexture> bones{};
	};
}
