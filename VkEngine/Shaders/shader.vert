#version 450
#extension GL_KHR_vulkan_glsl : enable

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inTexCoords;
layout(location = 2) in mat4 model;

layout(location = 0) out Data
{
    vec2 fragTexCoord;
    vec3 fragPos; 
} outData;

void main() 
{
    outData.fragTexCoord = inTexCoords;
    outData.fragPos = vec3(model * vec4(inPosition, 1.0));

    gl_Position = model * vec4(inPosition, 1);
}