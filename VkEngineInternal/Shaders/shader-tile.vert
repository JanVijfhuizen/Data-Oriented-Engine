#version 450
#extension GL_KHR_vulkan_glsl : enable

layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec2 inTexCoords;

#include "utils.shader"

struct InstanceData
{
    vec2 position;
    vec2 shape;
    SubTexture subTexture;
};

layout(std140, set = 0, binding = 0) readonly buffer InstanceBuffer
{
	InstanceData instances[];
} instanceBuffer;

layout(push_constant) uniform PushConstants
{
    vec2 resolution;
    vec2 cameraPosition;
    float pixelSize;
    int entitySize;
} pushConstants;

layout(location = 0) out Data
{
    flat SubTexture subTexture;
    vec2 fragTexCoords;
    vec2 fragPos;
    vec2 shape;
} outData;

void HandleInstance(in InstanceData instance)
{
    outData.subTexture = instance.subTexture;
    outData.fragTexCoords = inTexCoords;
    outData.fragPos = inPosition;
    outData.shape = instance.shape;

    gl_Position = CalculatePosition(instance.position, instance.shape * pushConstants.entitySize, pushConstants.cameraPosition, inPosition, pushConstants.resolution, pushConstants.pixelSize);
}

void main() 
{
    HandleInstance(instanceBuffer.instances[gl_InstanceIndex]);
}