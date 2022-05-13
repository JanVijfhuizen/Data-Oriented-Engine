#pragma once

namespace game
{
	struct SubTexture final
	{
		glm::vec2 leftTop{};
		glm::vec2 rightBot{1};

		[[nodiscard]] SubTexture operator +(const SubTexture& a) const;
		[[nodiscard]] SubTexture operator -(const SubTexture& a) const;
		SubTexture& operator +=(const SubTexture& a);
		SubTexture& operator -=(const SubTexture& a);
	};
}