#version 450
#extension GL_KHR_vulkan_glsl : enable

layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec2 inTexCoords;

layout(location = 0) out Data
{
    vec2 fragTexCoord;
    vec2 fragPos;
} outData;

void main() 
{
    outData.fragTexCoord = inTexCoords;
    outData.fragPos = inPosition;

    gl_Position = vec4(inPosition, 1, 1);
}