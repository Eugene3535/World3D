#version 460 core

uniform sampler2D circleSampler;

in vec2 texCoord;

layout(location = 0) out vec4 FragColor;

void main()
{
    vec4 texColor = texture(circleSampler, texCoord);

    if(texColor.a < 0.1)
        discard;

    FragColor = texColor;
}