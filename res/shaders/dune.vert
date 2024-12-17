#version 460 core

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec2 vertexTexCoord;

uniform mat4 MVP;

out vec2 mapTexCoord;
out vec2 terrainTexCoord;

void main()
{
    gl_Position = MVP * vec4(vertexPosition, 1.0f);
    mapTexCoord = vertexTexCoord;
    terrainTexCoord = vertexTexCoord * 0.1f;
}