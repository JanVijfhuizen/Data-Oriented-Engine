#ifndef UTILS
#define UTILS
vec2 Rotate(vec2 pos, float rotation)
{
	float newX = pos.x * cos(rotation) - pos.y * sin(rotation);
	float newY = pos.x * sin(rotation) + pos.y * cos(rotation);
	return vec2(newX, newY);
}
#endif
