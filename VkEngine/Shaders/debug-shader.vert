#version 450
#extension GL_KHR_vulkan_glsl : enable

layout(location = 0) in int inIndex;

struct InstanceData
{
    vec2 positions[2];
    vec3 color;
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

layout(location = 0) out Data
{
    vec3 color;
} outData;

void HandleInstance(in InstanceData instance)
{
    outData.color = instance.color;

    float aspectFix = pushConstants.resolution.y / pushConstants.resolution.x;
    vec2 worldPos = (instance.positions[inIndex] - pushConstants.cameraPosition) * pushConstants.pixelSize;
    vec4 worldPosv4 = vec4(worldPos, 1, 1);
    worldPosv4 *= aspectFix;

    gl_Position = worldPosv4;
}

void main() 
{
    HandleInstance(instanceBuffer.instances[gl_InstanceIndex]);
}