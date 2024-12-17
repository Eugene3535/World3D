#version 460 core

uniform sampler2D texMap;
uniform sampler2D texSand;
uniform sampler2D texSpice;
uniform sampler2D texStone;
uniform sampler2D texRock;

in vec2 mapTexCoord;

layout(location = 0) out vec4 FragColor;

void main()
{
    vec4 mask  = texture(texMap,   mapTexCoord);
    vec4 sand  = texture(texSand,  mapTexCoord);
    vec4 spice = texture(texSpice, mapTexCoord);
    vec4 stone = texture(texStone, mapTexCoord);
    vec4 rock  = texture(texRock,  mapTexCoord);

    float sandWeight = clamp(1.0f - (mask.r + mask.g + mask.b), 0.0f, 1.0f);
    FragColor = sand * sandWeight + spice * mask.r + stone * mask.g + rock * mask.b;
}