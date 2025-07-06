#version 460 core

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec2 vertexUV;
layout(location = 2) in vec3 vertexNormal;

layout (std140) uniform Matrices
{
    mat4 model_view_projection;
};

uniform mat4 model_view_matrix;
uniform mat3 normal_matrix;

out vec2 uv;
out vec3 normal;
out vec3 fragment_position;

void main()
{
    uv = vertexUV;
    normal = normalize(normal_matrix * vertexNormal);
    fragment_position = vec3(model_view_matrix * vec4(vertexPosition, 1.f));
    gl_Position = model_view_projection * vec4(vertexPosition, 1.f);
}