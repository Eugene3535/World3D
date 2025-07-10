#version 460 core

layout (location = 0) in vec3 vertexUV;

out vec3 uv;

layout (std140) uniform Matrices
{
    mat4 model_view_projection;
};

void main()
{
    uv = vertexUV;
    vec4 pos = model_view_projection * vec4(vertexUV, 1.0);
    gl_Position = pos.xyww;
}  