#version 450
#extension GL_KHR_vulkan_glsl : enable

layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec2 inTexCoords;

struct InstanceData
{
	vec2 position;
    float rotation;
    float scale;
};

layout(set = 0, binding = 0) readonly buffer InstanceBuffer
{
	InstanceData instances[];
} instanceBuffer;

layout(location = 0) out Data
{
    vec2 fragTexCoord;
    vec2 fragPos;
} outData;

void main() 
{
    outData.fragTexCoord = inTexCoords;
    outData.fragPos = inPosition;

    gl_Position = vec4(inPosition + instanceBuffer.instances[gl_InstanceIndex].position, 1, 1);
}