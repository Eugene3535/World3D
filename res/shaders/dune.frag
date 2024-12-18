#version 460 core

uniform sampler2D texMap;
uniform sampler2D texSand;
uniform sampler2D texSpice;
uniform sampler2D texStone;
uniform sampler2D texRock;

in vec2 mapTexCoord;
in vec2 sandTexCoord;
in vec2 spiceTexCoord;
in vec2 stoneTexCoord;
in vec2 rockTexCoord;

layout(location = 0) out vec4 FragColor;

void main()
{
    vec4 mask  = texture(texMap,   mapTexCoord);
    vec4 sand  = texture(texSand,  sandTexCoord);
    vec4 spice = texture(texSpice, spiceTexCoord);
    vec4 stone = texture(texStone, stoneTexCoord);
    vec4 rock  = texture(texRock,  rockTexCoord);

    float sandWeight = clamp(1.0f - (mask.r + mask.g + mask.b), 0.0f, 1.0f);
    FragColor = sand * sandWeight + spice * mask.r + stone * mask.g + rock * mask.b;
}