#pragma once
#include "Array.h"

namespace jlb
{
	class StringView;

	/// <summary>
	/// Simple class that is able to load and read files.
	/// </summary>
	class FileLoader final
	{
	public:
		static Array<char> Read(LinearAllocator& allocator, StringView path);
	};
}
