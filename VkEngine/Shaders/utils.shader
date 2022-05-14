#ifndef UTILS
#define UTILS
struct Transform 
{
    vec2 position;
    float rotation;
    float scale;
};

struct SubTexture
{
    vec2 lTop;
    vec2 rBot;
};

vec2 Rotate(in vec2 pos, in float rotation)
{
	float newX = pos.x * cos(rotation) - pos.y * sin(rotation);
	float newY = pos.x * sin(rotation) + pos.y * cos(rotation);
	return vec2(newX, newY);
}

vec2 CalculateTextureCoordinates(in SubTexture subTexture, in vec2 texCoords)
{
    return subTexture.lTop + (subTexture.rBot - subTexture.lTop) * texCoords;
}

vec4 CalculatePosition(in Transform transform, in vec2 camPosition, in vec2 vertPosition, in vec2 resolution, in float pixelSize)
{
    float aspectFix = resolution.y / resolution.x;
    vec2 worldPos = Rotate(transform.position - camPosition, transform.rotation) * pixelSize;
    vec4 pos = vec4(vertPosition * transform.scale * pixelSize + worldPos, 1, 1);
    pos.x *= aspectFix;
    return pos;
}
#endif
