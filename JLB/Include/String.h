#pragma once
#include "Array.h"
#include "StringView.h"

namespace jlb
{
	class String final : public Array<char>
	{
	public:
		void Allocate(StackAllocator& allocator, const StringView& view);
		void Allocate(StackAllocator& allocator, size_t size, const char& fillValue = {}) override;
		char& operator[](size_t index) const override;
		[[nodiscard]] StringView GetStringView() const;

	private:
		using Array<char>::begin;
		using Array<char>::end;
	};
}
