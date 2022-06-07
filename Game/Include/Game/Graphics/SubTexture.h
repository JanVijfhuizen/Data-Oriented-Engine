#pragma once

namespace game
{
	struct SubTexture final
	{
		union
		{
			struct
			{
				glm::vec2 leftTop;
				glm::vec2 rightBot;
			};
			glm::vec2 values[2]
			{
				glm::vec2{0},
				glm::vec2{1}
			};
		};

		[[nodiscard]] SubTexture operator +(const SubTexture& a) const;
		[[nodiscard]] SubTexture operator -(const SubTexture& a) const;
		SubTexture& operator +=(const SubTexture& a);
		SubTexture& operator -=(const SubTexture& a);
	};
}