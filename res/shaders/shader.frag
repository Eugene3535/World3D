#version 460 core

out vec4 FragColor;

void main()
{
    // FragColor = texture2D(texture0, tex_coord) * outColor;
    FragColor = vec4(0, 1, 1, 1);
}