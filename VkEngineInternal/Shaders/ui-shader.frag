#version 450
#extension GL_KHR_vulkan_glsl : enable

layout(set = 0, binding = 1) uniform sampler2D textureAtlas;

layout(location = 0) in Data
{
    vec2 fragTexCoord;
    vec2 fragPos;
} inData;

layout(location = 0) out vec4 outColor;

void main() 
{
    vec4 color = texture(textureAtlas, inData.fragTexCoord);
    if(color.a < .01f)
        discard;
    outColor = color;
}