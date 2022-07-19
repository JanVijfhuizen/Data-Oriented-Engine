#version 450
#extension GL_KHR_vulkan_glsl : enable

layout(set = 0, binding = 1) uniform sampler2D textureAtlas;

#include "utils.shader"

layout(location = 0) in Data
{
    flat SubTexture subTexture;
    vec2 fragTexCoords;
    vec2 fragPos;
    vec2 shape;
} inData;

layout(location = 0) out vec4 outColor;

void main() 
{
    vec2 texCoords = CalculateTextureCoordinates(inData.subTexture, fract(inData.fragTexCoords * inData.shape));
    vec4 color = texture(textureAtlas, texCoords);
    if(color.a < .01f)
        discard;
    outColor = color;
}