#pragma once
#include "Array.h"

namespace jlb
{
	class StringView;

	class FileLoader final
	{
	public:
		static Array<char> Read(LinearAllocator& allocator, StringView path);
	};
}
