#version 450
#extension GL_KHR_vulkan_glsl : enable

layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec2 inTexCoords;

struct InstanceData
{
    // Transform.
	vec2 position;
    float rotation;
    float scale;
    // Texture sub coordinates.
    vec2 texLTop;
    vec2 texRBot;
};

layout(std140, set = 0, binding = 0) readonly buffer InstanceBuffer
{
	InstanceData instances[];
} instanceBuffer;

layout(location = 0) out Data
{
    vec2 fragTexCoord;
    vec2 fragPos;
} outData;

void HandleInstance(in InstanceData instance)
{
    outData.fragTexCoord = instance.texLTop + (instance.texRBot - instance.texLTop) * inTexCoords;
    outData.fragPos = inPosition;
    gl_Position = vec4(inPosition + instance.position, 1, 1);
}

void main() 
{
    HandleInstance(instanceBuffer.instances[gl_InstanceIndex]);
}