#version 460 core

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec2 vertexTexCoord;

uniform mat4 MVP;

out vec2 mapTexCoord;
out vec2 sandTexCoord;
out vec2 spiceTexCoord;
out vec2 stoneTexCoord;
out vec2 rockTexCoord;

void main()
{
    gl_Position = MVP * vec4(vertexPosition, 1.0f);
    mapTexCoord = vertexTexCoord;
    sandTexCoord = mapTexCoord * 3;
    spiceTexCoord = mapTexCoord * 6;
    stoneTexCoord = mapTexCoord * 8;
    rockTexCoord = mapTexCoord * 8;
}