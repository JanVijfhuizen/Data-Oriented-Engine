#pragma once

namespace vke
{
	struct Camera final
	{
		glm::vec2 position{};
		// Size of a pixel, where the window width equals 1.
		float pixelSize = 0.008f;
	};
}