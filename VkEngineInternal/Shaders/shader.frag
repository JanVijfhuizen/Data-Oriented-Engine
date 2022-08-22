#version 450
#extension GL_KHR_vulkan_glsl : enable

layout(set = 0, binding = 1) uniform sampler2D textureAtlas;

layout(location = 0) in Data
{
    vec2 fragTexCoord;
    vec2 fragPos;
    flat vec3 lightDir;
    vec2 vertPos;
    int entitySize;
} inData;

layout(location = 0) out vec4 outColor;

void main() 
{
    vec2 pct = inData.vertPos;
    pct *= 16;
    ivec2 rounded = ivec2(pct);
    vec2 step = vec2(1) / 16;
    vec2 nPos = vec2(rounded) * step;

    vec3 normal = vec3(nPos.x * 2.f, nPos.y * 2.f, 1.f - length(nPos) * 2.f);
    normal = normalize(normal);

    float normAngle = dot(normal, inData.lightDir);
    float diff = max(normAngle, 0.0);
    vec3 diffuse = diff * vec3(1);

    vec4 color = texture(textureAtlas, inData.fragTexCoord) * vec4(diffuse, 1);
    if(color.a < .01f)
        discard;
    outColor = color;
}