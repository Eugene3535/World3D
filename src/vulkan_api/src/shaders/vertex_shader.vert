#version 460

layout(binding = 0) uniform UniformBufferObject 
{
    mat4 modelViewProjection;
} ubo;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inTexCoord;

layout(location = 0) out vec2 fragTexCoord;

void main() 
{
    gl_Position = ubo.modelViewProjection * vec4(inPosition, 1.f);
    fragTexCoord = inTexCoord;
}