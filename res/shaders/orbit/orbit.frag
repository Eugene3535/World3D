#version 460 core

uniform sampler2D texture0;

in vec2 texCoords;

layout(location = 0) out vec4 FragColor;

void main()
{
    FragColor = texture(texture0, texCoords);
}