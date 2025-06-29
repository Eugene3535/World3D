#version 460 core

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec2 vertexUV;
layout(location = 2) in vec3 vertexNormal;

layout (std140) uniform Matrices
{
    mat4 modelViewProjection;
};

uniform mat4 model_view_matrix = mat4(1);

out vec2 uv;
out vec3 normal;
out vec3 fragment_position;

void main()
{
    gl_Position = modelViewProjection * vec4(vertexPosition, 1.0f);
    uv = vertexUV;
    normal = vertexNormal;
    fragment_position = vec3(model_view_matrix * vec4(vertexPosition, 1f));
}