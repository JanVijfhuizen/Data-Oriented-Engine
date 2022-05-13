#include "pch.h"
#include "Graphics/SubTexture.h"

namespace game
{
	SubTexture SubTexture::operator+(const SubTexture& a) const
	{
		SubTexture b = *this;
		b += a;
		return b;
	}

	SubTexture SubTexture::operator-(const SubTexture& a) const
	{
		SubTexture b = *this;
		b -= a;
		return b;
	}

	SubTexture& SubTexture::operator+=(const SubTexture& a)
	{
		leftTop += a.leftTop;
		rightBot += a.rightBot;
		return *this;
	}

	SubTexture& SubTexture::operator-=(const SubTexture& a)
	{
		leftTop -= a.leftTop;
		rightBot -= a.rightBot;
		return *this;
	}
}
