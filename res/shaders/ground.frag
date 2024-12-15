#version 460 core

layout(location = 0) out vec4 FragColor;

smooth in vec2 ioVertexTexCoord;
smooth in vec2 ioPathTexCoord;

uniform sampler2D snowSampler;
uniform sampler2D pavementSampler;
uniform sampler2D pathSampler;

void main()
{
    vec4 snowTexel     = texture(snowSampler, ioVertexTexCoord);
    vec4 pavementTexel = texture(pavementSampler, ioVertexTexCoord);
    vec4 pathTexel     = texture(pathSampler, ioPathTexCoord);

    float pathWeight = pathTexel.r;
    float snowWeight = 1.0f - pathWeight;
    FragColor = pavementTexel * pathWeight + snowTexel * snowWeight;
}