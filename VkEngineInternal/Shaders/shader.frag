#version 450
#extension GL_KHR_vulkan_glsl : enable

layout(set = 0, binding = 1) uniform sampler2D textureAtlas;

layout(location = 0) in Data
{
    vec2 fragTexCoord;
    vec2 fragPos;
    flat vec3 lightDir;
    vec2 vertPos;
    flat int entitySize;
    flat float textureWidth;
} inData;

layout(location = 0) out vec4 outColor;

vec3 CalculateNormal()
{
    vec2 pct = inData.vertPos;
    pct *= inData.entitySize;
    ivec2 rounded = ivec2(pct);
    vec2 step = vec2(1) / inData.entitySize;
    vec2 nPos = vec2(rounded) * step;

    vec3 normal = vec3(nPos.x * 2.f, nPos.y * 2.f, 1.f - length(nPos) * 2.f);
    normal = normalize(normal);
    return normal;
}

vec3 CalculateDiffuse()
{
    vec3 normal = CalculateNormal();

    float normAngle = dot(normal, inData.lightDir);
    float diff = max(normAngle, 0.0);
    vec3 diffuse = diff * vec3(1);
    return diffuse;
}

bool IsEdge()
{
    float offset = inData.textureWidth / inData.entitySize;
    float a = 0;
    a += texture(textureAtlas, inData.fragTexCoord + vec2(offset, 0)).a;
    a += texture(textureAtlas, inData.fragTexCoord + vec2(-offset, 0)).a;
    a += texture(textureAtlas, inData.fragTexCoord + vec2(0, offset)).a;
    a += texture(textureAtlas, inData.fragTexCoord + vec2(0, -offset)).a;
    return a > 0;
}

void main() 
{
    vec3 diffuse = CalculateDiffuse();
    vec4 color = texture(textureAtlas, inData.fragTexCoord) * vec4(diffuse, 1);
    if(color.a < .01f)
    {
        if(!IsEdge())
            discard;
        color = vec4(vec3(0), 1);
    }
       
    outColor = color;
}