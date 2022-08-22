#version 450
#extension GL_KHR_vulkan_glsl : enable

layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec2 inTexCoords;

#include "utils.shader"

struct InstanceData
{
    Transform transform;
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
    vec3 lightDir;
    float pixelSize;
    int entitySize;
} pushConstants;

layout(location = 0) out Data
{
    vec2 fragTexCoord;
    vec2 fragPos;
    flat vec3 lightDir;
    vec2 vertPos;
    int entitySize;
} outData;

void HandleInstance(in InstanceData instance)
{
    outData.lightDir = pushConstants.lightDir;
    outData.vertPos = Rotate(inPosition, instance.transform.rotation);
    outData.entitySize = pushConstants.entitySize;

    outData.fragTexCoord = CalculateTextureCoordinates(instance.subTexture, inTexCoords);
    outData.fragPos = inPosition;

    gl_Position = CalculatePosition(instance.transform, pushConstants.cameraPosition, inPosition, pushConstants.resolution, pushConstants.pixelSize * pushConstants.entitySize);
}

void main() 
{
    HandleInstance(instanceBuffer.instances[gl_InstanceIndex]);
}