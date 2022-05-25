#pragma once
#include "Array.h"

namespace jlb
{
	class StringView;
}

namespace jlb
{
	class StringView;

	namespace file
	{
		/// <summary>
		/// Read a file and return the content as a array of chars.
		/// </summary>
		Array<char> Read(StackAllocator& allocator, StringView path);
	}
}
