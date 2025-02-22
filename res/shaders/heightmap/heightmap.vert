#version 460 core

layout(location = 0) in vec3 vertex;
layout(location = 1) in vec2 uv;

layout (std140) uniform Matrices
{
    mat4 modelViewProjection;
};

out vec2 texCoord;
out float heightPoint;

void main()
{
    gl_Position = modelViewProjection * vec4(vertex, 1.0f);

    texCoord = uv;
    heightPoint = vertex.y;
}