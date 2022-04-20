#version 450
#extension GL_KHR_vulkan_glsl : enable

layout(location = 0) in Data
{
    vec2 fragTexCoord;
    vec2 fragPos;
} inData;

layout(location = 0) out vec4 outColor;

void main() 
{
    outColor = vec4(inData.fragTexCoord, 0, 1);
}