#version 460 core

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec2 vertexTexCoord;

uniform mat4 MVP = mat4(1.0f);

smooth out vec2 ioVertexTexCoord;
smooth out vec2 ioPathTexCoord;

void main()
{
    gl_Position = MVP * vec4(vertexPosition, 1.0);
    ioVertexTexCoord = vertexTexCoord;
    ioPathTexCoord = vertexTexCoord * 0.05f;
}