#pragma once

namespace game::math
{
	constexpr auto PI = 3.14159265358979323846;

	[[nodiscard]] float GetAngle(const glm::vec2& a, const glm::vec2& b);
	[[nodiscard]] float SmoothAngle(float a, float b, float delta);
	[[nodiscard]] float WrapAngle(float f);
	[[nodiscard]] float Lerp(float a, float b, float delta);
	[[nodiscard]] float LerpPct(float a, float b, float pct);
}