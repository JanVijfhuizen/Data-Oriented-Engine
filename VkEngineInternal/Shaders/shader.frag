#version 450
#extension GL_KHR_vulkan_glsl : enable

layout(set = 0, binding = 1) uniform sampler2D textureAtlas;

layout(location = 0) in Data
{
    vec2 fragTexCoord;
    vec2 fragPos;
    flat vec3 lightDir;
    vec2 vertPos;
} inData;

layout(location = 0) out vec4 outColor;

void main() 
{
    vec3 normal = vec3(inData.vertPos.x * 2.f, inData.vertPos.y * 2.f, length(inData.vertPos));
    normal = normalize(normal);

    float diff = max(dot(normal, inData.lightDir), 0.0);
    vec3 diffuse = diff * vec3(1);

    vec4 color = texture(textureAtlas, inData.fragTexCoord) * vec4(diffuse, 1);
    if(color.a < .01f)
        discard;
    outColor = color;
}