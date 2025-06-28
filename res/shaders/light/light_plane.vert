#version 460 core

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec2 vertexTexCoord;

layout (std140) uniform Matrices
{
    mat4 modelViewProjection;
};

out vec2 texCoords;

void main()
{
    gl_Position = modelViewProjection * vec4(vertexPosition, 1.0f);
    texCoords = vertexTexCoord;
}