#include "JlbString.h"
#include "JlbMath.h"

namespace jlb
{
	void String::AllocateFromNumber(StackAllocator& allocator, size_t number)
	{
		size_t d = 1;
		size_t length = 0;
		while (d <= number)
		{
			d *= 10;
			++length;
		}

		Array<char>::Allocate(allocator, length + 1);
		const auto data = GetData();
		data[length] = '\0';

		d = 10;
		for (size_t j = 0; j < length; ++j)
		{
			auto& c = data[length - 1 - j];
			const size_t mod = number % d;
			const size_t n = mod * 10 / d;
			c = '0' + n;
			d *= 10;
		}
	}

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
