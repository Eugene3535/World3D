#version 460 core

layout(location = 0) out vec4 outputColor;

smooth in vec2 ioVertexTexCoord;

uniform sampler2D snowSampler;
uniform sampler2D pavementSampler;
uniform sampler2D pathSampler;

void main()
{
    vec4 snowTexel     = texture(snowSampler, ioVertexTexCoord);
    vec4 pavementTexel = texture(pavementSampler, ioVertexTexCoord);
    vec4 pathTexel     = texture(pathSampler, ioVertexTexCoord / 20.0f);

    float pathWeight = pathTexel.r;
    float snowWeight = 1.0 - pathWeight;
    outputColor = pavementTexel * pathWeight + snowTexel * snowWeight;
}