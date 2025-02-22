#version 460 core

layout(location = 0) in vec4 vertex;

layout (std140) uniform Matrices
{
    mat4 modelViewProjection;
};

out vec2 texCoord;

void main()
{
    gl_Position = modelViewProjection * vec4(vertex.xy, 0.0f, 1.0f);

    texCoord = vertex.zw;
}