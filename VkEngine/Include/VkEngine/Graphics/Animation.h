#pragma once

namespace vke
{
	struct SubTexture;

	struct Animation
	{
		float lerp = 0;
		bool repeat = true;
		size_t width = 1;

		[[nodiscard]] SubTexture Evaluate(SubTexture subTexture, float delta);
	};
}
