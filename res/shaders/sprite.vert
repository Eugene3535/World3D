#version 460 core

layout (location = 0) in vec2 position;
layout (location = 1) in vec2 tex_coords;

layout (std140) uniform Matrices
{
    mat4 modelViewProjection;
};

out vec2 texCoords;

void main()
{
    gl_Position = modelViewProjection * vec4(position.x, position.y, 0.0f, 1.0f);
    
    texCoords = tex_coords;
}
