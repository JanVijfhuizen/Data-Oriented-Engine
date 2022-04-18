#version 450
#extension GL_KHR_vulkan_glsl : enable

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inTexCoords;
layout(location = 2) in vec3 instancePosition;
layout(location = 3) in float instanceRotation;
layout(location = 4) in float instanceScale;

layout(location = 0) out Data
{
    vec2 fragTexCoord;
    vec3 fragPos; 
} outData;

void main() 
{
    outData.fragTexCoord = inTexCoords;
    outData.fragPos = inPosition;

    gl_Position = vec4(inPosition, 1);
}