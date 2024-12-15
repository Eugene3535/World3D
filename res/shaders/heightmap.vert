#version 460 core

layout(location = 0) in vec3 vertex;
layout(location = 1) in vec2 uv;

out vec2 texCoord;
out float heightPoint;

uniform mat4 MVP = mat4(1.0f);

void main()
{
    gl_Position = MVP * vec4(vertex, 1.0f);

    texCoord = uv;
    heightPoint = vertex.y;
}