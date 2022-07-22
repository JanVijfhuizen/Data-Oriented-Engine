#include "String.h"
#include "JlbMath.h"

namespace jlb
{
	void String::Allocate(StackAllocator& allocator, const StringView& view)
	{
		Array<char>::Allocate(allocator, view.GetLength() + 1);
		const auto data = GetData();
		const size_t length = GetLength() - 1;
		for (size_t i = 0; i < length; ++i)
			data[i] = view[i];
		data[length] = '\0';
	}

	void String::Allocate(StackAllocator& allocator, const size_t size, const char& fillValue)
	{
		Array<char>::Allocate(allocator, size, fillValue);
		GetData()[GetLength() - 1] = '\0';
	}

	char& String::operator[](const size_t index) const
	{
		assert(index < GetLength() - 1);
		return Array<char>::operator[](index);
	}

	StringView String::GetStringView() const
	{
		const StringView view{GetData()};
		return view;
	}
}
