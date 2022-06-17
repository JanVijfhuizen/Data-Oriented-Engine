#version 450
#extension GL_KHR_vulkan_glsl : enable

layout(location = 0) in int inIndex;

#include "utils.shader"

struct InstanceData
{
	vec2 start;
	vec2 end;
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
} pushConstants;

void HandleInstance(in InstanceData instance)
{
    gl_Position = CalculatePoint(instance.start * inIndex + instance.end * (1 - inIndex), pushConstants.cameraPosition, pushConstants.resolution, pushConstants.pixelSize);
}

void main() 
{
    HandleInstance(instanceBuffer.instances[gl_InstanceIndex]);
}