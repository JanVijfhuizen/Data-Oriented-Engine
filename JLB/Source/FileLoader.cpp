#include "FileLoader.h"
#include <fstream>
#include <cassert>
#include "StringView.h"

namespace jlb
{
	Array<char> file::Read(StackAllocator& allocator, const StringView path)
	{
		std::ifstream file(path, std::ios::ate | std::ios::binary);

		assert(file.is_open());

		// Dump contents in the buffer.
		const size_t fileSize = static_cast<size_t>(file.tellg());
		Array<char> buffer{};
		buffer.Allocate(allocator, fileSize);

		file.seekg(0);
		file.read(buffer.GetView().data, fileSize);
		file.close();
		return buffer;
	}
}
