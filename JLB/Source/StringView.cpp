#include "StringView.h"

namespace jlb
{
	StringView::StringView(const char* strLit) : _strLit(strLit)
	{

	}

	const char* StringView::GetData() const
	{
		return _strLit;
	}

	bool StringView::operator==(StringView& other)
	{
		return _strLit == other._strLit;
	}

	bool StringView::operator==(const char* other)
	{
		return _strLit == other;
	}

	bool StringView::operator!=(StringView& other)
	{
		return !operator==(other);
	}

	bool StringView::operator!=(const char* other)
	{
		return !operator==(other);
	}

	StringView::operator const char* () const
	{
		return _strLit;
	}
}