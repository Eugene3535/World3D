#version 460 core

in vec2 texCoords;
out vec4 FragColor;

uniform sampler2D texture0;
uniform int hp;
uniform vec4 borderThickness;

void main()
{
    vec4 texColor = texture(texture0, texCoords);
    const int maxHp = 100;
    
    if(texCoords.x < borderThickness.x || texCoords.y < borderThickness.y ||
       texCoords.x > borderThickness.z || texCoords.y > borderThickness.w)
    {
        FragColor = texColor;
    }
    else
    {
        if(hp > 0 && hp < maxHp)
        {
            float h = borderThickness.w - borderThickness.y;
            float k = float(hp) * 0.01f;
            float n = h * k;

            if(borderThickness.w - n > texCoords.y)
                FragColor = vec4(0.f, 0.f, 0.f, 1.f);
            else
                FragColor = texColor;
        }
        else
        {
            FragColor = texColor;
        }
    }
}